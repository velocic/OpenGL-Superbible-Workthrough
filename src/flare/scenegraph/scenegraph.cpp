#include <flare/scenegraph/scenegraph.h>

#include <algorithm>
#include <unordered_set>
#include <queue>

#include <flare/rendersystem/factory.h>

#include <glm-0.9.9/gtx/matrix_decompose.hpp>

namespace Flare
{
    namespace SceneGraph
    {
        size_t SceneGraph::requestName()
        {
            return nextNameToAssign++;
        }

        SceneGraph::SceneGraph()
        {
            rootNode = std::unique_ptr<Node>(new Node(*this, requestName(), nullptr));
            auto commandBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
                .addAttribute("count", 1, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, 0)
                .addAttribute("instanceCount", 1, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(RenderSystem::RSuint))
                .addAttribute("firstIndex", 1, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(RenderSystem::RSuint) * 2)
                .addAttribute("baseVertex", 1, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(RenderSystem::RSuint) * 3)
                .addAttribute("baseInstance", 1, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(RenderSystem::RSuint) * 4)
                .setStride(sizeof(RenderSystem::DrawElementsIndirectCommand))
                .build();
            indirectRenderCommandsBuffer.create(
                "indirectRenderCommands",
                commandBufferLayout
            );
            indirectRenderCommandsBuffer.get()->allocateBufferStorage(
                32 * sizeof(RenderSystem::DrawElementsIndirectCommand),
                nullptr,
                RenderSystem::RSbitfield{} |
                    RenderSystem::RS_DYNAMIC_STORAGE_BIT |
                    RenderSystem::RS_MAP_WRITE_BIT
            );
        }

        SceneGraph::SceneGraph(SceneGraph &&other)
        :
            indirectRenderCommandsBuffer(std::move(other.indirectRenderCommandsBuffer)),
            rootNode(std::move(other.rootNode)),
            nextNameToAssign(std::exchange(other.nextNameToAssign, 0))
        {
        }

        SceneGraph &SceneGraph::operator=(SceneGraph &&other)
        {
            indirectRenderCommandsBuffer = (std::move(other.indirectRenderCommandsBuffer));
            rootNode = std::move(other.rootNode);
            nextNameToAssign = std::exchange(other.nextNameToAssign, 0);

            return *this;
        }

        void SceneGraph::destroy()
        {
            rootNode = nullptr;
        }

        Node *SceneGraph::getRootNode() const
        {
            return rootNode.get();
        }

        void SceneGraph::render()
        {
            rootNode->render(Math::identityMatrix);
        }

        void SceneGraph::renderIndirect()
        {
            using CommandGroupRange = std::pair<size_t, size_t>;

            auto getMaterialId = [](const auto &textures) -> size_t {
                if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(textures)) {
                    return std::get<RenderSystem::PhongMaterialTextures>(textures).materialId;
                } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(textures)) {
                    return std::get<RenderSystem::PBRMaterialTextures>(textures).materialId;
                }
                return -1;
            };

            auto bindMaterialTextures = [](auto &shader, auto &textures){
                if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(textures)) {
                    const auto &phongTextures = std::get<RenderSystem::PhongMaterialTextures>(textures);
                    for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.diffuse.size(); ++materialTextureIndex) {
                        shader->setTexture("textureDiffuse" + std::to_string(materialTextureIndex), phongTextures.diffuse[materialTextureIndex]);
                    }
                    for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.specular.size(); ++materialTextureIndex) {
                        shader->setTexture("textureSpecular" + std::to_string(materialTextureIndex), phongTextures.specular[materialTextureIndex]);
                    }
                    for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.normal.size(); ++materialTextureIndex) {
                        shader->setTexture("textureNormal" + std::to_string(materialTextureIndex), phongTextures.normal[materialTextureIndex]);
                    }
                } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(textures)) {
                    const auto &PBRTextures = std::get<RenderSystem::PBRMaterialTextures>(textures);
                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.baseColor.size(); ++materialTextureIndex) {
                        shader->setTexture("textureBaseColor" + std::to_string(materialTextureIndex), PBRTextures.baseColor[materialTextureIndex]);
                    }
                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.normal.size(); ++materialTextureIndex) {
                        shader->setTexture("textureNormal" + std::to_string(materialTextureIndex), PBRTextures.normal[materialTextureIndex]);
                    }
                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.metallic.size(); ++materialTextureIndex) {
                        shader->setTexture("textureMetallic" + std::to_string(materialTextureIndex), PBRTextures.metallic[materialTextureIndex]);
                    }
                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.roughness.size(); ++materialTextureIndex) {
                        shader->setTexture("textureRoughness" + std::to_string(materialTextureIndex), PBRTextures.roughness[materialTextureIndex]);
                    }
                }
            };

            auto sortDrawCommandsByMaterial = [](auto &unsortedDrawCommands, const auto &getMaterialId){
                std::sort(
                    unsortedDrawCommands.begin(),
                    unsortedDrawCommands.end(),
                    [&getMaterialId](const auto &command1, const auto &command2){
                        auto command1MaterialId = getMaterialId(command1.textures);
                        auto command2MaterialId = getMaterialId(command2.textures);

                        return (command1.shaderData.hashedAlias == command2.shaderData.hashedAlias)
                            && (command1MaterialId == command2MaterialId);
                    }
                );
            };

            auto sortDrawCommandRangeByMVPMatrixBuffer = [](auto beginIterator, auto endIterator){
                std::sort(
                    beginIterator,
                    endIterator,
                    [](const auto &command1, const auto &command2){
                        return command1.meshData.mvpMatrixBuffer->getName() == command2.meshData.mvpMatrixBuffer->getName();
                    }
                );
            };

            auto getCommandGroupRangeIndices = [](const auto &drawCommandsSortedByMaterial, const auto &getMaterialId){
                auto lastCommandGroupIndexStart = size_t{0};
                auto commandGroupRangeIndices = std::vector<std::pair<size_t, size_t>>{};

                if (drawCommandsSortedByMaterial.size() == 1) {
                    commandGroupRangeIndices.emplace_back(lastCommandGroupIndexStart, 0);
                    return commandGroupRangeIndices;
                }

                for (size_t i = 1; i < drawCommandsSortedByMaterial.size(); ++i) {
                    const auto &startOfRangeDrawCommand = drawCommandsSortedByMaterial[lastCommandGroupIndexStart];
                    const auto &currentDrawCommand = drawCommandsSortedByMaterial[i];
                    auto isLastIteration = i == drawCommandsSortedByMaterial.size() - 1;

                    if ((currentDrawCommand.shaderData.hashedAlias != startOfRangeDrawCommand.shaderData.hashedAlias)
                        || (getMaterialId(currentDrawCommand.textures) != getMaterialId(startOfRangeDrawCommand.textures))
                        || isLastIteration) {

                        if (isLastIteration) {
                            commandGroupRangeIndices.emplace_back(lastCommandGroupIndexStart, i);
                        } else {
                            commandGroupRangeIndices.emplace_back(lastCommandGroupIndexStart, i - 1);
                        }

                        lastCommandGroupIndexStart = i;
                    }
                }

                return commandGroupRangeIndices;
            };

            auto getCombinedRenderDataBuffers = [](auto &sortedDrawCommands, const auto &commandGroupRanges){
                auto mvpMatrixBuffer = RenderSystem::createBuffer("mvpMatrix", sortedDrawCommands[0].meshData.mvpMatrixBuffer->getContentDescription());
                auto vertexBuffer = RenderSystem::createBuffer("vertexBuffer", sortedDrawCommands[0].meshData.vertexBuffer->getContentDescription());
                auto elementBuffer = RenderSystem::createBuffer("elementBuffer", sortedDrawCommands[0].meshData.elementBuffer->getContentDescription());

                //calc required size for each buffer, then allocate storage
                auto combinedMVPMatrixBufferSize = size_t{0};
                auto combinedVertexBufferSize = size_t{0};
                auto combinedElementBufferSize = size_t{0};

                {
                    auto mvpMatrixBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
                    auto vertexBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
                    auto elementBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};

                    for (const auto &drawCommand : sortedDrawCommands) {
                        auto hasCountedThisMVPMatrixBuffer = mvpMatrixBuffersEncountered.find(drawCommand.meshData.mvpMatrixBuffer) != mvpMatrixBuffersEncountered.end();
                        auto hasCountedThisVertexBuffer = vertexBuffersEncountered.find(drawCommand.meshData.vertexBuffer) != vertexBuffersEncountered.end();
                        auto hasCountedThisElementBuffer = elementBuffersEncountered.find(drawCommand.meshData.elementBuffer) != elementBuffersEncountered.end();

                        if (!hasCountedThisMVPMatrixBuffer) {
                            combinedMVPMatrixBufferSize += drawCommand.meshData.mvpMatrixBuffer->getSizeInBytes();
                            mvpMatrixBuffersEncountered.insert(drawCommand.meshData.mvpMatrixBuffer);
                        }

                        if (!hasCountedThisVertexBuffer) {
                            combinedVertexBufferSize += drawCommand.meshData.vertexBuffer->getSizeInBytes();
                            vertexBuffersEncountered.insert(drawCommand.meshData.vertexBuffer);
                        }

                        if (!hasCountedThisElementBuffer) {
                            combinedElementBufferSize += drawCommand.meshData.elementBuffer->getSizeInBytes();
                            elementBuffersEncountered.insert(drawCommand.meshData.elementBuffer);
                        }
                    }
                }

                mvpMatrixBuffer->allocateBufferStorage(combinedMVPMatrixBufferSize, nullptr, 0);
                vertexBuffer->allocateBufferStorage(combinedVertexBufferSize, nullptr, 0);
                elementBuffer->allocateBufferStorage(combinedElementBufferSize, nullptr, 0);

                //copy separated buffers into combined buffers, and update DrawElementsIndirectCommand(s) to point to the
                //correct offsets within the new buffers
                auto mvpMatrixBufferBytesCopiedSoFar = size_t{0};
                auto vertexBufferBytesCopiedSoFar = size_t{0};
                auto elementBufferBytesCopiedSoFar = size_t{0};

                auto baseVertexWithinNewBuffer = size_t{0};
                auto firstIndexWithinNewBuffer = size_t{0};

                auto mvpMatrixBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
                auto vertexBufferToBaseVertex = std::unordered_map<const RenderSystem::Buffer *, size_t>{};
                auto elementBufferToFirstIndex = std::unordered_map<const RenderSystem::Buffer *, size_t>{};

                for (size_t currentRange = 0; currentRange < commandGroupRanges.size(); ++currentRange) {
                    const auto &commandGroupRange = commandGroupRanges[currentRange];

                    for (size_t drawCommandIndex = commandGroupRange.first; drawCommandIndex < commandGroupRange.second + 1; ++drawCommandIndex) {
                        auto &drawCommand = sortedDrawCommands[drawCommandIndex];

                        const auto &sourceMVPMatrixBuffer = *drawCommand.meshData.mvpMatrixBuffer;
                        const auto &sourceVertexBuffer = *drawCommand.meshData.vertexBuffer;
                        const auto &sourceElementBuffer = *drawCommand.meshData.elementBuffer;

                        auto mvpMatrixBufferSearchIterator = mvpMatrixBuffersEncountered.find(drawCommand.meshData.mvpMatrixBuffer);
                        auto vertexBufferSearchIterator = vertexBufferToBaseVertex.find(drawCommand.meshData.vertexBuffer);
                        auto elementBufferSearchIterator = elementBufferToFirstIndex.find(drawCommand.meshData.elementBuffer);

                        auto hasNotCopiedThisMVPMatrixBuffer = mvpMatrixBufferSearchIterator == mvpMatrixBuffersEncountered.end();
                        auto hasNotCopiedThisVertexBuffer = vertexBufferSearchIterator == vertexBufferToBaseVertex.end();
                        auto hasNotCopiedThisElementBuffer = elementBufferSearchIterator == elementBufferToFirstIndex.end();

                        if (hasNotCopiedThisMVPMatrixBuffer) {
                            mvpMatrixBuffersEncountered.insert(drawCommand.meshData.mvpMatrixBuffer);
                            mvpMatrixBuffer->copyRange(sourceMVPMatrixBuffer, 0, mvpMatrixBufferBytesCopiedSoFar, sourceMVPMatrixBuffer.getSizeInBytes());

                            drawCommand.drawElementsIndirectCommand.baseInstance = mvpMatrixBufferBytesCopiedSoFar / sizeof(glm::mat4);

                            mvpMatrixBufferBytesCopiedSoFar += sourceMVPMatrixBuffer.getSizeInBytes();
                        } else {
                            auto relativeBaseInstance = drawCommand.drawElementsIndirectCommand.baseInstance;
                            auto offsetWithinCombinedBuffer = mvpMatrixBufferBytesCopiedSoFar - sourceMVPMatrixBuffer.getSizeInBytes();
                            drawCommand.drawElementsIndirectCommand.baseInstance = relativeBaseInstance + offsetWithinCombinedBuffer / sizeof(glm::mat4);
                        }

                        if (hasNotCopiedThisVertexBuffer) {
                            vertexBufferToBaseVertex.insert_or_assign(drawCommand.meshData.vertexBuffer, baseVertexWithinNewBuffer);
                            vertexBuffer->copyRange(sourceVertexBuffer, 0, vertexBufferBytesCopiedSoFar, sourceVertexBuffer.getSizeInBytes());
                            drawCommand.drawElementsIndirectCommand.baseVertex = baseVertexWithinNewBuffer + drawCommand.drawElementsIndirectCommand.baseVertex;

                            vertexBufferBytesCopiedSoFar += sourceVertexBuffer.getSizeInBytes();
                            baseVertexWithinNewBuffer += sourceVertexBuffer.getSizeInElements();
                        } else {
                            auto meshStartWithinCombinedBuffer = vertexBufferSearchIterator->second;
                            drawCommand.drawElementsIndirectCommand.baseVertex = meshStartWithinCombinedBuffer + drawCommand.drawElementsIndirectCommand.baseVertex;
                        }

                        if (hasNotCopiedThisElementBuffer) {
                            elementBufferToFirstIndex.insert_or_assign(drawCommand.meshData.elementBuffer, firstIndexWithinNewBuffer);
                            elementBuffer->copyRange(sourceElementBuffer, 0, elementBufferBytesCopiedSoFar, sourceElementBuffer.getSizeInBytes());
                            drawCommand.drawElementsIndirectCommand.firstIndex = firstIndexWithinNewBuffer + drawCommand.drawElementsIndirectCommand.firstIndex;

                            elementBufferBytesCopiedSoFar += sourceElementBuffer.getSizeInBytes();
                            firstIndexWithinNewBuffer += sourceElementBuffer.getSizeInElements();
                        } else {
                            auto indicesStartWithinCombinedBuffer = elementBufferSearchIterator->second;
                            drawCommand.drawElementsIndirectCommand.firstIndex = indicesStartWithinCombinedBuffer + drawCommand.drawElementsIndirectCommand.firstIndex;
                        }
                    }
                }

                return std::make_tuple(std::move(mvpMatrixBuffer), std::move(vertexBuffer), std::move(elementBuffer));
            };

            auto sortedDrawCommands = rootNode->getIndirectDrawCommands(Math::identityMatrix);
            if (sortedDrawCommands.size() == 0) {
                return;
            }

            sortDrawCommandsByMaterial(sortedDrawCommands, getMaterialId);
            auto commandGroupRanges = getCommandGroupRangeIndices(sortedDrawCommands, getMaterialId);
            auto commandGroupQueue = std::queue<CommandGroupRange>(std::deque<CommandGroupRange>(commandGroupRanges.begin(), commandGroupRanges.end()));

            //Sort each group of related render commands so that each command that shares a set of buffers is adjacent
            for (size_t currentRange = 0; currentRange < commandGroupRanges.size(); ++currentRange) {
                const auto &range = commandGroupRanges[currentRange];

                sortDrawCommandRangeByMVPMatrixBuffer(sortedDrawCommands.begin() + range.first, sortedDrawCommands.begin() + range.second + 1);
            }

            auto [combinedMVPMatrixBuffer, combinedVertexBuffer, combinedElementBuffer] = getCombinedRenderDataBuffers(sortedDrawCommands, commandGroupRanges);

            //Guarantee the indirect commands GPU buffer is at least large enough to fit the current command set
            if (indirectRenderCommandsBuffer.get()->getSizeInElements() < sortedDrawCommands.size()) {
                indirectRenderCommandsBuffer.resizeElements(indirectRenderCommandsBuffer.get()->getSizeInElements() * 2);
            }

            //Fill indirect render commands GPU buffer
            auto indirectRenderCommands = std::vector<RenderSystem::DrawElementsIndirectCommand>{};
            indirectRenderCommands.reserve(sortedDrawCommands.size());
            for (const auto &drawCommand : sortedDrawCommands) {
                indirectRenderCommands.push_back(drawCommand.drawElementsIndirectCommand);
            }
            indirectRenderCommandsBuffer.get()->bufferRange(
                0,
                indirectRenderCommands.size() * sizeof(RenderSystem::DrawElementsIndirectCommand),
                indirectRenderCommands.data()
            );
            indirectRenderCommandsBuffer.get()->bind(RenderSystem::RS_DRAW_INDIRECT_BUFFER);

            //Submit the indirect render commands
            while (!commandGroupQueue.empty()) {
                const auto &drawCommandGroup = commandGroupQueue.front();
                const auto &firstDrawCommandInGroup = sortedDrawCommands[drawCommandGroup.first];

                //Bind shader & material for draw command group
                firstDrawCommandInGroup.shaderData.shader->bind();
                firstDrawCommandInGroup.shaderData.vertexArray->bind();
                bindMaterialTextures(firstDrawCommandInGroup.shaderData.shader, firstDrawCommandInGroup.textures);

                combinedElementBuffer->bind(RenderSystem::RS_ELEMENT_ARRAY_BUFFER);
                firstDrawCommandInGroup.shaderData.vertexArray->linkBuffers(
                    std::vector<std::reference_wrapper<const RenderSystem::Buffer>>{*combinedVertexBuffer, *combinedMVPMatrixBuffer}
                );

                //TODO: abstract the GL call here into a platform-independent wrapper
                glMultiDrawElementsIndirect(
                    RenderSystem::RS_TRIANGLES,
                    RenderSystem::RS_UNSIGNED_INT,
                    reinterpret_cast<void *>(drawCommandGroup.first * sizeof(RenderSystem::DrawElementsIndirectCommand)),
                    (drawCommandGroup.second - drawCommandGroup.first) + 1,//number of draw commands in the group
                    0
                );

                commandGroupQueue.pop();
            }
        }

        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent)
        {
            setParallelBufferSizes(1);
            instanceData.instanceIdLookupTable[getNextInstanceId()] = 0;
        }

        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent, Model *model)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent),
            model(model)
        {
            setParallelBufferSizes(1);
            instanceData.instanceIdLookupTable[getNextInstanceId()] = 0;
        }

        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent, size_t instanceCountReserveSize)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent)
        {
            setParallelBufferSizes(instanceCountReserveSize);
            for (size_t i = 0; i < instanceCountReserveSize; ++i) {
                instanceData.instanceIdLookupTable[getNextInstanceId()] = i;
            }
        }

        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent, size_t instanceCountReserveSize, Model *model)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent),
            model(model)
        {
            setParallelBufferSizes(instanceCountReserveSize);
            for (size_t i = 0; i < instanceCountReserveSize; ++i) {
                instanceData.instanceIdLookupTable[getNextInstanceId()] = i;
            }
        }

        Node::Node(Node &&other)
        :
            nextInstanceIdToAssign(std::exchange(other.nextInstanceIdToAssign, 0)),
            TRSData(std::move(other.TRSData)),
            instanceData(std::move(other.instanceData)),
            children(std::move(other.children)),
            name(std::exchange(other.name, 0)),
            shaderData(std::move(other.shaderData)),
            userProvidedShaderBuffers(std::move(other.userProvidedShaderBuffers)),
            modelMatrixBuffer(std::move(other.modelMatrixBuffer)),
            sceneGraph(std::exchange(other.sceneGraph, nullptr)),
            parent(std::exchange(other.parent, nullptr)),
            model(std::exchange(other.model, nullptr))
        {
        }

        Node &Node::operator=(Node &&other)
        {
            nextInstanceIdToAssign = std::exchange(other.nextInstanceIdToAssign, 0);
            TRSData = std::move(other.TRSData);
            instanceData = std::move(other.instanceData);
            children = std::move(other.children);
            name = std::exchange(other.name, 0);
            shaderData = std::move(other.shaderData);
            userProvidedShaderBuffers = std::move(other.userProvidedShaderBuffers);
            modelMatrixBuffer = std::move(other.modelMatrixBuffer);
            sceneGraph = std::exchange(other.sceneGraph, nullptr);
            parent = std::exchange(other.parent, nullptr);
            model = std::exchange(other.model, nullptr);

            return *this;
        }

        DataTypes::DecomposedModelMatrix Node::decomposeInstance(size_t instanceId) const
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to decompose an invalid instanceId");
            }

            auto result = DataTypes::DecomposedModelMatrix{};
            glm::decompose(
                getInstanceLocalTransform(instanceId),
                result.scale,
                result.orientation,
                result.translation,
                result.skew,
                result.perspective
            );

            return result;
        }

        DataTypes::DecomposedModelMatrix Node::decomposeNode() const
        {
            auto result = DataTypes::DecomposedModelMatrix{};

            glm::decompose(
                getNodeLocalTransform(),
                result.scale,
                result.orientation,
                result.translation,
                result.skew,
                result.perspective
            );

            return result;
        }

        void Node::destroy()
        {
            for (auto &child : children) {
                child->destroy();
            }

            //Delete this instance's backing data (which is owned by this instance's parent)
            if (parent != nullptr) {
                auto thisInstancesOwningPointerIterator = std::find_if(
                    parent->children.begin(),
                    parent->children.end(),
                    [&](const auto &parentsChild){
                        return parentsChild->getName() == getName();
                    }
                );
                if (thisInstancesOwningPointerIterator != parent->children.end()) {
                    parent->children.erase(thisInstancesOwningPointerIterator);
                }
            }
        }

        size_t Node::getName() const
        {
            return name;
        }

        glm::mat4 Node::getNodeLocalTransform() const
        {
            return TRSData.translation * TRSData.rotation * TRSData.scale;
        }

        glm::mat4 Node::getInstanceLocalTransform(size_t instanceId) const
        {
            const auto &instance = instanceData.TRSData[instanceId];
            return getNodeLocalTransform()
                * instance.translation * instance.rotation * instance.scale;
        }

        void Node::setModel(Model *newModel)
        {
            model = newModel;

            //Resize the modelMatrix buffer if the new model contains more than 1 mesh
            if (model->getMeshCount() != 1) {
                setParallelBufferSizes(std::max(instanceData.numActive, size_t{1}));
            }
        }

        void Node::setShaderData(RenderSystem::ShaderData newShaderData)
        {
            shaderData = newShaderData;
        }

        void Node::setShaderBuffers(const std::vector<const RenderSystem::Buffer *> &buffers)
        {
            userProvidedShaderBuffers = buffers;
        }

        void Node::translateNode(const glm::vec3 &translation)
        {
            TRSData.translation = glm::translate(
                TRSData.translation,
                translation
            );
        }

        void Node::rotateNode(float angleRadians, const glm::vec3 &axis)
        {
            TRSData.rotation = glm::rotate(
                TRSData.rotation,
                angleRadians,
                axis
            );
        }

        void Node::scaleNode(const glm::vec3 &scale)
        {
            TRSData.scale = glm::scale(
                TRSData.scale,
                scale
            );
        }

        void Node::translateInstance(size_t instanceId, const glm::vec3 &translation)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to translate an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.translation = glm::translate(
                instance.translation,
                translation
            );
        }

        void Node::rotateInstance(size_t instanceId, float angleRadians, const glm::vec3 &axis)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to rotate an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.rotation = glm::rotate(
                instance.rotation,
                angleRadians,
                axis
            );
        }

        void Node::scaleInstance(size_t instanceId, const glm::vec3 &scale)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to scale an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.scale = glm::scale(
                instance.scale,
                scale
            );
        }

        void Node::setNodePosition(const glm::vec3 &position)
        {
            TRSData.translation = glm::translate(
                Math::identityMatrix,
                position
            );
        }

        void Node::setNodeRotation(float angleRadians, const glm::vec3 &axis)
        {
            TRSData.rotation = glm::rotate(
                Math::identityMatrix,
                angleRadians,
                axis
            );
        }

        void Node::setNodeScale(const glm::vec3 &scale)
        {
            TRSData.scale = glm::scale(
                Math::identityMatrix,
                scale
            );
        }

        void Node::setInstancePosition(size_t instanceId, const glm::vec3 &position)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to set position on an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.translation = glm::translate(
                Math::identityMatrix,
                position
            );
        }

        void Node::setInstanceRotation(size_t instanceId, float angleRadians, const glm::vec3 &axis)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to set rotation on an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.rotation = glm::rotate(
                Math::identityMatrix,
                angleRadians,
                axis
            );
        }

        void Node::setInstanceScale(size_t instanceId, const glm::vec3 &scale)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to set scale on an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.scale = glm::scale(
                Math::identityMatrix,
                scale
            );
        }

        size_t Node::addInstance()
        {
            auto newInstanceId = getNextInstanceId();

            auto setInstanceToDefaultValues = [&](auto index){
                instanceData.TRSData[index] = TranslateRotateScaleData{};
                instanceData.instanceIdLookupTable[newInstanceId] = index;
            };

            if (modelMatrixBuffer.get() == nullptr) {
                setParallelBufferSizes(1);
                instanceData.numActive = 1;
                setInstanceToDefaultValues(0);
                return newInstanceId;
            }

            const auto modelMatrixBufferSize = modelMatrixBuffer.get()->getSizeInElements();

            if (instanceData.numActive < modelMatrixBufferSize) {
                setInstanceToDefaultValues(instanceData.numActive);
                ++instanceData.numActive;
                return newInstanceId;
            }

            const auto newModelMatrixBufferSize = modelMatrixBufferSize * 2;
            setParallelBufferSizes(newModelMatrixBufferSize);
            setInstanceToDefaultValues(instanceData.numActive);
            ++instanceData.numActive;

            return newInstanceId;
        }

        Node *Node::createChildNode()
        {
            auto newChild = std::unique_ptr<Node>(new Node(*sceneGraph, sceneGraph->requestName(), this));
            auto result = newChild.get();
            children.push_back(std::move(newChild));

            return result;
        }

        void Node::addChildNode(Node *child)
        {
            if (child->parent == nullptr) {
                throw std::runtime_error("Attempted to re-parent the root scene node (which is disallowed) or the passed argument has no parent (which is only allowed for the root scene node)");
            }

            if (child->getName() == getName()) {
                throw std::runtime_error("Attempted to re-parent a scene node to itself");
            }

            if (child->parent->getName() == getName()) {
                //Attempting to reparent to the same parent; nothing to do.
                return;
            }

            auto &oldParentNode = *child->parent;
            auto childOwningPointerIterator = std::find_if(
                oldParentNode.children.begin(),
                oldParentNode.children.end(),
                [&](const auto &oldParentsChild){
                    return oldParentsChild->getName() == child->getName();
                }
            );

            child->parent = this;
            children.push_back(std::move(*childOwningPointerIterator));
            oldParentNode.children.erase(childOwningPointerIterator);
        }

        Node *Node::copy() const
        {
            if (parent == nullptr) {
                throw std::runtime_error("Attempted to copy the root scene node (which is disallowed)");
            }

            auto copyNode = std::unique_ptr<Node>(new Node(*sceneGraph, sceneGraph->requestName(), parent));
            copyNode->TRSData = TRSData;
            copyNode->instanceData = instanceData;
            copyNode->model = model;
            copyNode->shaderData = shaderData;
            copyNode->deepCopyChildrenOfOtherNode(copyNode->children, *this);
            copyNode->copyModelMatrixBufferOfOtherNode(*this);

            auto result = copyNode.get();
            parent->children.push_back(std::move(copyNode));

            return result;
        }

        void Node::removeInstance(size_t instanceId)
        {
            if (instanceData.numActive == 0) {
                return;
            }

            auto removeIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (removeIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to remove an invalid instanceId");
            }
            auto removeIndex = removeIndexIterator->second;

            auto lastActiveInstanceDataIndex = instanceData.numActive - 1;

            std::iter_swap(
                instanceData.TRSData.begin() + removeIndex,
                instanceData.TRSData.begin() + lastActiveInstanceDataIndex
            );
            instanceData.instanceIdLookupTable.erase(instanceId);
            --instanceData.numActive;

            if (instanceData.numActive == 0) {
                modelMatrixBuffer.destroy();
            }
        }

        void Node::removeAllInstances()
        {
            instanceData.modelMatrices.clear();
            instanceData.TRSData.clear();
            instanceData.numActive = 0;
            modelMatrixBuffer.destroy();
        }

        void Node::removeAllChildren()
        {
            children.clear();
        }

        void Node::render(const glm::mat4 &parentModelMatrix)
        {
            const auto localCoordinateSpace = parentModelMatrix * TRSData.translation * TRSData.rotation * TRSData.scale;

            if (model != nullptr && modelMatrixBuffer.get() != nullptr && instanceData.numActive > 0) {
                updateModelMatrixBuffer(localCoordinateSpace, model->getMeshLocalTransforms());

                model->render(shaderData, *modelMatrixBuffer.get(), instanceData.numActive);
            }

            for (auto &child : children) {
                child->render(localCoordinateSpace);
            }
        }

        std::vector<Mesh::SortableDrawElementsIndirectCommand> Node::getIndirectDrawCommands(const glm::mat4 &parentModelMatrix)
        {
            const auto localCoordinateSpace = parentModelMatrix * TRSData.translation * TRSData.rotation * TRSData.scale;

            auto accumulatedResults = std::vector<Mesh::SortableDrawElementsIndirectCommand>{};

            if (model != nullptr && modelMatrixBuffer.get() != nullptr && instanceData.numActive > 0) {
                updateModelMatrixBuffer(localCoordinateSpace, model->getMeshLocalTransforms());
                auto modelDrawCommands = model->getIndirectDrawCommands(shaderData, *modelMatrixBuffer.get(), instanceData.numActive);
                std::move(modelDrawCommands.begin(), modelDrawCommands.end(), std::back_inserter(accumulatedResults));
            }

            for (auto &child : children) {
                auto childAccumulatedDrawCommands = child->getIndirectDrawCommands(localCoordinateSpace);
                std::move(childAccumulatedDrawCommands.begin(), childAccumulatedDrawCommands.end(), std::back_inserter(accumulatedResults));
            }

            return accumulatedResults;
        }

        void Node::copyModelMatrixBufferOfOtherNode(const Node &other)
        {
            if (modelMatrixBuffer.get() != nullptr) {
                modelMatrixBuffer.destroy();
            }

            modelMatrixBuffer.create(
                mvpMatrixBufferName,
                other.modelMatrixBuffer.get()->getContentDescription()
            );
            modelMatrixBuffer.get()->allocateBufferStorage(
                other.modelMatrixBuffer.get()->getSizeInBytes(),
                nullptr,
                other.modelMatrixBuffer.get()->getUsageFlags()
            );
            modelMatrixBuffer.get()->copyRange(
                *other.modelMatrixBuffer.get(),
                0,
                0,
                other.modelMatrixBuffer.get()->getSizeInBytes()
            );
        }

        void Node::deepCopyChildrenOfOtherNode(std::vector<std::unique_ptr<Node>> &destination, const Node &other)
        {
            for (const auto &child : other.children) {
                auto newNode = std::unique_ptr<Node>(new Node(*sceneGraph, sceneGraph->requestName(), parent));
                newNode->TRSData = child->TRSData;
                newNode->instanceData = child->instanceData;
                newNode->model = child->model;
                newNode->shaderData = child->shaderData;
                newNode->copyModelMatrixBufferOfOtherNode(*child);
                destination.push_back(std::move(newNode));
                deepCopyChildrenOfOtherNode(newNode->children, *child);
            }
        }

        size_t Node::getNextInstanceId()
        {
            return nextInstanceIdToAssign++;
        }

        void Node::updateModelMatrixBuffer(const glm::mat4 &parentModelMatrix, const std::vector<glm::mat4> &subMeshLocalTransforms)
        {
            if (instanceData.numActive <= 0) {
                return;
            }

            for (size_t subMeshIndex = 0; subMeshIndex < subMeshLocalTransforms.size(); ++subMeshIndex) {
                const auto &subMeshLocalTransform = subMeshLocalTransforms[subMeshIndex];

                for (size_t instanceDataBaseIndex = 0; instanceDataBaseIndex < instanceData.numActive; ++instanceDataBaseIndex) {
                    auto bufferOutputIndex = instanceDataBaseIndex + instanceData.numActive * subMeshIndex;

                    instanceData.modelMatrices[bufferOutputIndex] =
                        parentModelMatrix
                        * instanceData.TRSData[instanceDataBaseIndex].translation
                        * instanceData.TRSData[instanceDataBaseIndex].rotation
                        * instanceData.TRSData[instanceDataBaseIndex].scale
                        * subMeshLocalTransform;
                }
            }

            modelMatrixBuffer.get()->bufferRange(
                0,
                sizeof(glm::mat4) * instanceData.modelMatrices.size(),
                instanceData.modelMatrices.data()
            );
        }

        RenderSystem::VertexDataLayout Node::getModelMatrixBufferLayout() const
        {
            return Flare::RenderSystem::VertexDataLayoutBuilder()
                .addMatrixAttribute("mvpMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
                .setStride(sizeof(glm::mat4))
                .setDivisor(1)
                .build();
        }

        void Node::setParallelBufferSizes(size_t size)
        {
            instanceData.TRSData.resize(size);

            //Create room for the model matrices for all submeshes in a model (which probably have their own local transforms
            //relative to the model's origin)
            auto modelMatrixBufferSize = size;
            if (model != nullptr) {
                modelMatrixBufferSize = size * model->getMeshCount();
            }

            instanceData.modelMatrices.resize(modelMatrixBufferSize);

            if (modelMatrixBuffer.get() != nullptr) {
                modelMatrixBuffer.resizeElements(modelMatrixBufferSize);
                return;
            }

            modelMatrixBuffer.create(
                mvpMatrixBufferName,
                getModelMatrixBufferLayout()
            )->allocateBufferStorage(
                sizeof(glm::mat4) * modelMatrixBufferSize,
                nullptr,
                getModelMatrixBufferUsageFlags()
            );
        }
    }
}
