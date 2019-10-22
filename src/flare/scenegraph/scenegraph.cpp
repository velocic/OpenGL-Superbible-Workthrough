#include <flare/scenegraph/scenegraph.h>

#include <algorithm>

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
            using MeshIdToDrawCommandsMap = std::unordered_map<size_t, std::vector<Mesh::SortableDrawElementsIndirectCommand>>;
            using MaterialIdToMeshMap = std::unordered_map<size_t, MeshIdToDrawCommandsMap>;
            using ShaderIdToMaterialMap = std::unordered_map<size_t, MaterialIdToMeshMap>;

            auto unsortedDrawCommands = rootNode->getIndirectDrawCommands(Math::identityMatrix);
            auto sortedDrawCommands = ShaderIdToMaterialMap{};
            auto indirectRenderCommands = std::vector<RenderSystem::DrawElementsIndirectCommand>{};
            indirectRenderCommands.resize(unsortedDrawCommands.size());

            for (const auto &unsortedDrawCommand : unsortedDrawCommands) {
                auto materialId = size_t{0};
                if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(unsortedDrawCommand.textures)) {
                    materialId = std::get<RenderSystem::PhongMaterialTextures>(unsortedDrawCommand.textures).materialId;
                } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(unsortedDrawCommand.textures)) {
                    materialId = std::get<RenderSystem::PBRMaterialTextures>(unsortedDrawCommand.textures).materialId;
                } else {
                    materialId = -1;
                }

                sortedDrawCommands[unsortedDrawCommand.shaderData.hashedAlias][materialId][unsortedDrawCommand.meshData.meshId].push_back(unsortedDrawCommand);
            }

            for (auto &shaderEntry : sortedDrawCommands) {
                auto shaderBound = false;
                for (auto &materialEntry : shaderEntry.second) {
                    auto materialTexturesBound = false;

                    for (auto &meshEntry : materialEntry.second) {
                        auto meshRenderDataBound = false;

                        auto totalDrawCommandsForShaderAndMaterial = meshEntry.second.size();
                        if (indirectRenderCommandsBuffer.get()->getSizeInElements() < totalDrawCommandsForShaderAndMaterial) {
                            indirectRenderCommandsBuffer.resizeElements(indirectRenderCommandsBuffer.get()->getSizeInElements() * 2);
                        }

                        auto drawCommandIndex = size_t{0};
                        indirectRenderCommandsBuffer.get()->bind(RenderSystem::RS_DRAW_INDIRECT_BUFFER);

                        for (auto &drawCommand : meshEntry.second) {
                            if (!shaderBound) {
                                drawCommand.shaderData.shader->bind();
                                drawCommand.shaderData.vertexArray->bind();
                                shaderBound = true;
                            }

                            if (!meshRenderDataBound) {
                                auto &meshData = meshEntry.second[0].meshData;
                                meshData.elementBuffer->bind(RenderSystem::RS_ELEMENT_ARRAY_BUFFER);
                                drawCommand.shaderData.vertexArray->linkBuffers(std::vector<std::reference_wrapper<const RenderSystem::Buffer>>{*meshData.vertexBuffer, *meshData.mvpMatrixBuffer});
                                meshRenderDataBound = true;
                            }

                            if (!materialTexturesBound) {
                                if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(meshEntry.second[0].textures)) {
                                    const auto &phongTextures = std::get<RenderSystem::PhongMaterialTextures>(meshEntry.second[0].textures);
                                    for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.diffuse.size(); ++materialTextureIndex) {
                                        drawCommand.shaderData.shader->setTexture("textureDiffuse" + std::to_string(materialTextureIndex), phongTextures.diffuse[materialTextureIndex]);
                                    }
                                    for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.specular.size(); ++materialTextureIndex) {
                                        drawCommand.shaderData.shader->setTexture("textureSpecular" + std::to_string(materialTextureIndex), phongTextures.specular[materialTextureIndex]);
                                    }
                                    for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.normal.size(); ++materialTextureIndex) {
                                        drawCommand.shaderData.shader->setTexture("textureNormal" + std::to_string(materialTextureIndex), phongTextures.normal[materialTextureIndex]);
                                    }
                                } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(meshEntry.second[0].textures)) {
                                    const auto &PBRTextures = std::get<RenderSystem::PBRMaterialTextures>(meshEntry.second[0].textures);

                                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.baseColor.size(); ++materialTextureIndex) {
                                        drawCommand.shaderData.shader->setTexture("textureBaseColor" + std::to_string(materialTextureIndex), PBRTextures.baseColor[materialTextureIndex]);
                                    }
                                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.normal.size(); ++materialTextureIndex) {
                                        drawCommand.shaderData.shader->setTexture("textureNormal" + std::to_string(materialTextureIndex), PBRTextures.normal[materialTextureIndex]);
                                    }
                                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.metallic.size(); ++materialTextureIndex) {
                                        drawCommand.shaderData.shader->setTexture("textureMetallic" + std::to_string(materialTextureIndex), PBRTextures.metallic[materialTextureIndex]);
                                    }
                                    for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.roughness.size(); ++materialTextureIndex) {
                                        drawCommand.shaderData.shader->setTexture("textureRoughness" + std::to_string(materialTextureIndex), PBRTextures.roughness[materialTextureIndex]);
                                    }
                                }
                                materialTexturesBound = true;
                            }

                            indirectRenderCommands[drawCommandIndex++] = drawCommand.drawElementsIndirectCommand;
                        }

                        indirectRenderCommandsBuffer.get()->bufferRange(0, totalDrawCommandsForShaderAndMaterial, indirectRenderCommands.data());
                        //TODO: abstract the GL call here into a platform-independent wrapper
                        glMultiDrawElementsIndirect(
                            RenderSystem::RS_TRIANGLES,
                            RenderSystem::RS_UNSIGNED_INT,
                            0,
                            totalDrawCommandsForShaderAndMaterial,
                            0
                        );
                    }
                }
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
            TRSData(std::move(other.TRSData)),
            instanceData(std::move(other.instanceData)),
            children(std::move(other.children)),
            name(std::exchange(other.name, 0)),
            modelMatrixBuffer(std::move(other.modelMatrixBuffer)),
            sceneGraph(std::exchange(other.sceneGraph, nullptr)),
            parent(std::exchange(other.parent, nullptr)),
            model(std::exchange(other.model, nullptr))
        {
        }

        Node &Node::operator=(Node &&other)
        {
            TRSData = std::move(other.TRSData);
            instanceData = std::move(other.instanceData);
            children = std::move(other.children);
            name = std::exchange(other.name, 0);
            sceneGraph = std::exchange(other.sceneGraph, nullptr);
            modelMatrixBuffer = std::move(other.modelMatrixBuffer);
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
        }

        void Node::setShaderData(RenderSystem::ShaderData newShaderData)
        {
            shaderData = newShaderData;
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
                instanceData.modelMatrices[index] = glm::mat4{};
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
                instanceData.modelMatrices.begin() + removeIndex,
                instanceData.modelMatrices.begin() + lastActiveInstanceDataIndex
            );
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

            updateModelMatrixBuffer(localCoordinateSpace);

            if (model != nullptr && modelMatrixBuffer.get() != nullptr && instanceData.numActive > 0) {
                model->render(shaderData, *modelMatrixBuffer.get(), instanceData.numActive);
            }

            for (auto &child : children) {
                child->render(localCoordinateSpace);
            }
        }

        std::vector<Mesh::SortableDrawElementsIndirectCommand> Node::getIndirectDrawCommands(const glm::mat4 &parentModelMatrix)
        {
            const auto localCoordinateSpace = parentModelMatrix * TRSData.translation * TRSData.rotation * TRSData.scale;
            updateModelMatrixBuffer(localCoordinateSpace);

            auto accumulatedResults = std::vector<Mesh::SortableDrawElementsIndirectCommand>{};

            if (model != nullptr && modelMatrixBuffer.get() != nullptr && instanceData.numActive > 0) {
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

        void Node::updateModelMatrixBuffer(const glm::mat4 &parentModelMatrix)
        {
            if (instanceData.numActive <= 0) {
                return;
            }

            for (size_t i = 0; i < instanceData.numActive; ++i) {
                instanceData.modelMatrices[i] =
                    parentModelMatrix
                    * instanceData.TRSData[i].translation
                    * instanceData.TRSData[i].rotation
                    * instanceData.TRSData[i].scale;
            }

            modelMatrixBuffer.get()->bufferRange(
                0,
                sizeof(glm::mat4) * instanceData.numActive,
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
            instanceData.modelMatrices.resize(size);
            instanceData.TRSData.resize(size);
            if (modelMatrixBuffer.get() != nullptr) {
                modelMatrixBuffer.resizeElements(size);
                return;
            }

            modelMatrixBuffer.create(
                mvpMatrixBufferName,
                getModelMatrixBufferLayout()
            );
            modelMatrixBuffer.get()->allocateBufferStorage(
                sizeof(glm::mat4) * size,
                nullptr,
                getModelMatrixBufferUsageFlags()
            );
        }
    }
}
