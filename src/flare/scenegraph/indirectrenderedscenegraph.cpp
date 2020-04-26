#include <flare/scenegraph/indirectrenderedscenegraph.h>

#include <queue>
#include <unordered_set>
#include <variant>

#include <flare/scenegraph/node.h>
#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace SceneGraph
    {
        IndirectRenderedSceneGraph::IndirectRenderedSceneGraph()
        {
            rootNode = std::unique_ptr<Node>(new Node(*this, requestName(), nullptr));
            auto commandBufferLayout = RenderSystem::VertexDataLayoutBuilder()
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
                RenderSystem::RSbitfield{} | RenderSystem::RS_DYNAMIC_STORAGE_BIT
            );
        }

        IndirectRenderedSceneGraph::~IndirectRenderedSceneGraph()
        {
        }

        IndirectRenderedSceneGraph::IndirectRenderedSceneGraph(IndirectRenderedSceneGraph &&other)
        :
            indirectRenderCommandsBuffer(std::move(other.indirectRenderCommandsBuffer)),
            rootNode(std::move(other.rootNode)),
            nextNameToAssign(std::exchange(other.nextNameToAssign, 0))
        {
        }

        IndirectRenderedSceneGraph &IndirectRenderedSceneGraph::operator=(IndirectRenderedSceneGraph &&other)
        {
            indirectRenderCommandsBuffer = (std::move(other.indirectRenderCommandsBuffer));
            rootNode = std::move(other.rootNode);
            nextNameToAssign = std::exchange(other.nextNameToAssign, 0);

            return *this;
        }

        void IndirectRenderedSceneGraph::destroy()
        {
            rootNode = nullptr;
        }

        Node *IndirectRenderedSceneGraph::getRootNode() const
        {
            return rootNode.get();
        }

        void IndirectRenderedSceneGraph::render()
        {
            auto sortedDrawCommands = rootNode->getIndirectDrawCommands(Math::identityMatrix);
            if (sortedDrawCommands.size() == 0) {
                return;
            }

            auto shaderGroups = sortDrawCommandsByShader(sortedDrawCommands);

            for (auto &shaderGroup : shaderGroups) {
                sortDrawCommandsWithinShaderGroupByMaterial(sortedDrawCommands, shaderGroup);

                for (auto &materialGroup : shaderGroup.materialGroups) {
                    sortDrawCommandsWithinMaterialGroupByMVPMatrixBuffer(sortedDrawCommands, materialGroup);
                }
            }

            for (const auto &shaderGroup : shaderGroups) {
                auto hasUserProvidedShaderBuffers = sortedDrawCommands[shaderGroup.firstIndex].meshData.userProvidedShaderBuffers != nullptr &&
                    sortedDrawCommands[shaderGroup.firstIndex].meshData.userProvidedShaderBuffers->size() != 0;

                if (hasUserProvidedShaderBuffers == false) {
                    combineBuffersAndMultiDrawEachMaterialGroup(sortedDrawCommands, shaderGroup);
                } else {
                    linkUserProvidedShaderBuffersAndRenderDrawCommandsIndividually(sortedDrawCommands, shaderGroup);
                }
            }
        }

        size_t IndirectRenderedSceneGraph::requestName()
        {
            return nextNameToAssign++;
        }

        size_t IndirectRenderedSceneGraph::getMaterialId(const MaterialTextures &textures) const
        {
            if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(textures)) {
                return std::get<RenderSystem::PhongMaterialTextures>(textures).materialId;
            } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(textures)) {
                return std::get<RenderSystem::PBRMaterialTextures>(textures).materialId;
            }

            return -1;
        }

        void IndirectRenderedSceneGraph::bindMaterialTextures(RenderSystem::ShaderProgram &shader, const MaterialTextures &textures)
        {
            if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(textures)) {
                const auto &phongTextures = std::get<RenderSystem::PhongMaterialTextures>(textures);
                for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.diffuse.size(); ++materialTextureIndex) {
                    shader.setTexture("textureDiffuse" + std::to_string(materialTextureIndex), phongTextures.diffuse[materialTextureIndex]);
                }
                for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.specular.size(); ++materialTextureIndex) {
                    shader.setTexture("textureSpecular" + std::to_string(materialTextureIndex), phongTextures.specular[materialTextureIndex]);
                }
                for (size_t materialTextureIndex = 0; materialTextureIndex < phongTextures.normal.size(); ++materialTextureIndex) {
                    shader.setTexture("textureNormal" + std::to_string(materialTextureIndex), phongTextures.normal[materialTextureIndex]);
                }
            } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(textures)) {
                const auto &PBRTextures = std::get<RenderSystem::PBRMaterialTextures>(textures);
                for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.baseColor.size(); ++materialTextureIndex) {
                    shader.setTexture("textureBaseColor" + std::to_string(materialTextureIndex), PBRTextures.baseColor[materialTextureIndex]);
                }
                for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.normal.size(); ++materialTextureIndex) {
                    shader.setTexture("textureNormal" + std::to_string(materialTextureIndex), PBRTextures.normal[materialTextureIndex]);
                }
                for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.metallic.size(); ++materialTextureIndex) {
                    shader.setTexture("textureMetallic" + std::to_string(materialTextureIndex), PBRTextures.metallic[materialTextureIndex]);
                }
                for (size_t materialTextureIndex = 0; materialTextureIndex < PBRTextures.roughness.size(); ++materialTextureIndex) {
                    shader.setTexture("textureRoughness" + std::to_string(materialTextureIndex), PBRTextures.roughness[materialTextureIndex]);
                }
            }
        }

        std::vector<IndirectRenderedSceneGraph::ShaderGroup> IndirectRenderedSceneGraph::sortDrawCommandsByShader(SortableDrawCommands &unsortedDrawCommands)
        {
            auto result = std::vector<ShaderGroup>{};

            if (unsortedDrawCommands.size() == 0) {
                return result;
            }

            if (unsortedDrawCommands.size() == 1) {
                result.push_back(ShaderGroup{0, 0, std::vector<MaterialGroup>{}});
                return result;
            }

            std::sort(
                unsortedDrawCommands.begin(),
                unsortedDrawCommands.end(),
                [](const auto &command1, const auto &command2){
                    return command1.shaderData.hashedAlias == command2.shaderData.hashedAlias;
                }
            );

            auto lastEncounteredShaderId = unsortedDrawCommands[0].shaderData.hashedAlias;
            auto indexOfLastEncounteredShader = size_t{0};

            for (size_t i = 0; i < unsortedDrawCommands.size(); ++i) {
                auto isLastIteration = i == unsortedDrawCommands.size() - 1;

                if (isLastIteration) {
                    result.push_back(ShaderGroup{indexOfLastEncounteredShader, i, std::vector<MaterialGroup>{}});
                    continue;
                }

                if (unsortedDrawCommands[i].shaderData.hashedAlias != lastEncounteredShaderId) {
                    result.push_back(ShaderGroup{indexOfLastEncounteredShader, i - 1, std::vector<MaterialGroup>{}});

                    lastEncounteredShaderId = unsortedDrawCommands[i].shaderData.hashedAlias;
                    indexOfLastEncounteredShader = i;
                }
            }

            return result;
        }

        void IndirectRenderedSceneGraph::sortDrawCommandsWithinShaderGroupByMaterial(SortableDrawCommands &unsortedDrawCommands, ShaderGroup &shaderGroup)
        {
            std::sort(
                unsortedDrawCommands.begin() + shaderGroup.firstIndex,
                unsortedDrawCommands.begin() + shaderGroup.lastIndex + 1,
                [this](const auto &command1, const auto &command2){
                    auto command1MaterialId = getMaterialId(command1.textures);
                    auto command2MaterialId = getMaterialId(command2.textures);

                    return (command1.shaderData.hashedAlias == command2.shaderData.hashedAlias)
                        && (command1MaterialId == command2MaterialId);
                }
            );

            shaderGroup.materialGroups = getMaterialGroups(unsortedDrawCommands, shaderGroup);
        }

        void IndirectRenderedSceneGraph::sortDrawCommandsWithinMaterialGroupByMVPMatrixBuffer(SortableDrawCommands &unsortedDrawCommands, MaterialGroup &materialGroup)
        {
            std::sort(
                unsortedDrawCommands.begin() + materialGroup.firstIndex,
                unsortedDrawCommands.begin() + materialGroup.lastIndex + 1,
                [](const auto &command1, const auto &command2){
                    return command1.meshData.mvpMatrixBuffer == command2.meshData.mvpMatrixBuffer;
                }
            );

            materialGroup.bufferGroups = getBufferGroups(unsortedDrawCommands, materialGroup);
        }

        std::vector<IndirectRenderedSceneGraph::MaterialGroup> IndirectRenderedSceneGraph::getMaterialGroups(const SortableDrawCommands &drawCommandsSortedByMaterial, const ShaderGroup &shaderGroup)
        {
            auto lastMaterialGroupIndexStart = shaderGroup.firstIndex;
            auto materialGroups = std::vector<MaterialGroup>{};

            auto numDrawCommandsInShaderGroup = (shaderGroup.lastIndex - shaderGroup.firstIndex) + 1;

            if (numDrawCommandsInShaderGroup == 1) {
                materialGroups.push_back(MaterialGroup{shaderGroup.firstIndex, shaderGroup.firstIndex, std::vector<BufferGroup>{}});
                return materialGroups;
            }

            for (size_t i = shaderGroup.firstIndex; i <= shaderGroup.lastIndex; ++i) {
                const auto &startOfRangeDrawCommand = drawCommandsSortedByMaterial[lastMaterialGroupIndexStart];
                const auto &currentDrawCommand = drawCommandsSortedByMaterial[i];
                auto isLastIteration = i == shaderGroup.lastIndex;

                auto drawCommandsAreInSameMaterialGroup = (currentDrawCommand.shaderData.hashedAlias == startOfRangeDrawCommand.shaderData.hashedAlias)
                    && (getMaterialId(currentDrawCommand.textures) == getMaterialId(startOfRangeDrawCommand.textures));

                if (isLastIteration) {
                    if (drawCommandsAreInSameMaterialGroup) {
                        materialGroups.push_back(MaterialGroup{lastMaterialGroupIndexStart, i, std::vector<BufferGroup>{}});
                        continue;
                    }

                    materialGroups.push_back(MaterialGroup{lastMaterialGroupIndexStart, i - 1, std::vector<BufferGroup>{}});
                    materialGroups.push_back(MaterialGroup{i, i, std::vector<BufferGroup>{}});
                } else if (!drawCommandsAreInSameMaterialGroup) {
                    materialGroups.push_back(MaterialGroup{lastMaterialGroupIndexStart, i - 1, std::vector<BufferGroup>{}});
                    lastMaterialGroupIndexStart = i;
                }
            }

            return materialGroups;
        }

        std::vector<IndirectRenderedSceneGraph::BufferGroup> IndirectRenderedSceneGraph::getBufferGroups(const SortableDrawCommands &drawCommandsSortedByMVPMatrixBuffer, const MaterialGroup &materialGroup)
        {
            auto lastBufferGroupIndexStart = materialGroup.firstIndex;
            auto bufferGroups = std::vector<BufferGroup>{};
            auto numDrawCommandsInMaterialGroup = (materialGroup.lastIndex - materialGroup.firstIndex) + 1;

            if (numDrawCommandsInMaterialGroup == 1) {
                bufferGroups.push_back(BufferGroup{materialGroup.firstIndex, materialGroup.firstIndex});
                return bufferGroups;
            }

            for (size_t i = materialGroup.firstIndex; i <= materialGroup.lastIndex; ++i) {
                const auto &startOfRangeDrawCommand = drawCommandsSortedByMVPMatrixBuffer[lastBufferGroupIndexStart];
                const auto &currentDrawCommand = drawCommandsSortedByMVPMatrixBuffer[i];
                auto isLastIteration = i == materialGroup.lastIndex;

                auto drawCommandsAreInSameBufferGroup = currentDrawCommand.meshData.mvpMatrixBuffer == startOfRangeDrawCommand.meshData.mvpMatrixBuffer;

                if (isLastIteration) {
                    if (drawCommandsAreInSameBufferGroup) {
                        bufferGroups.push_back(BufferGroup{lastBufferGroupIndexStart, i});
                        continue;
                    }

                    bufferGroups.push_back(BufferGroup{lastBufferGroupIndexStart, i - 1});
                    bufferGroups.push_back(BufferGroup{i, i});
                } else if (!drawCommandsAreInSameBufferGroup) {
                    bufferGroups.push_back(BufferGroup{lastBufferGroupIndexStart, i - 1});
                    lastBufferGroupIndexStart = i;
                }
            }

            return bufferGroups;
        }

        void IndirectRenderedSceneGraph::initializeShaderGroupRenderDataBuffers(const RenderSystem::VertexDataLayout &mvpMatrixBufferLayout, const RenderSystem::VertexDataLayout &vertexBufferLayout, const RenderSystem::VertexDataLayout &elementBufferLayout)
        {
            shaderGroupRenderDataBuffers.mvpMatrixBuffer.create("mvpMatrix", mvpMatrixBufferLayout);
            shaderGroupRenderDataBuffers.vertexBuffer.create("vertexBuffer", vertexBufferLayout);
            shaderGroupRenderDataBuffers.elementBuffer.create("elementBuffer", elementBufferLayout);

            shaderGroupRenderDataBuffers.mvpMatrixBuffer.get()->allocateBufferStorage(
                32 * sizeof(glm::mat4),
                nullptr,
                RenderSystem::RSbitfield{} | RenderSystem::RS_DYNAMIC_STORAGE_BIT
            );
            shaderGroupRenderDataBuffers.vertexBuffer.get()->allocateBufferStorage(
                32 * sizeof(DataTypes::Vertex),
                nullptr,
                RenderSystem::RSbitfield{} | RenderSystem::RS_DYNAMIC_STORAGE_BIT
            );
            shaderGroupRenderDataBuffers.elementBuffer.get()->allocateBufferStorage(
                32 * sizeof(unsigned int),
                nullptr,
                RenderSystem::RSbitfield{} | RenderSystem::RS_DYNAMIC_STORAGE_BIT
            );
            
            shaderGroupRenderDataBuffers.buffersHaveBeenInitialized = true;
        }

        void IndirectRenderedSceneGraph::fillShaderGroupRenderDataBuffers(SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup)
        {
            auto calcRequiredBufferSizes = [](const auto &drawCommands){
                auto combinedMVPMatrixBufferSize = size_t{0};
                auto combinedVertexBufferSize = size_t{0};
                auto combinedElementBufferSize = size_t{0};
                auto mvpMatrixBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
                auto vertexBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
                auto elementBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};

                for (const auto &drawCommand : drawCommands) {
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

                return std::make_tuple(combinedMVPMatrixBufferSize, combinedVertexBufferSize, combinedElementBufferSize);
            };

            auto resizeBuffersIfRequired = [this](auto mvpMatrixBufferSize, auto vertexBufferSize, auto elementBufferSize){
                if (shaderGroupRenderDataBuffers.mvpMatrixBuffer.get()->getSizeInBytes() < mvpMatrixBufferSize) {
                    shaderGroupRenderDataBuffers.mvpMatrixBuffer.resizeBytes(mvpMatrixBufferSize);
                }

                if (shaderGroupRenderDataBuffers.vertexBuffer.get()->getSizeInBytes() < vertexBufferSize) {
                    shaderGroupRenderDataBuffers.vertexBuffer.resizeBytes(vertexBufferSize);
                }

                if (shaderGroupRenderDataBuffers.elementBuffer.get()->getSizeInBytes() < elementBufferSize) {
                    shaderGroupRenderDataBuffers.elementBuffer.resizeBytes(elementBufferSize);
                }
            };

            if (sortedDrawCommands.size() == 0) {
                return;
            }

            if (!shaderGroupRenderDataBuffers.buffersHaveBeenInitialized) {
                initializeShaderGroupRenderDataBuffers(
                    sortedDrawCommands[0].meshData.mvpMatrixBuffer->getContentDescription(),
                    sortedDrawCommands[0].meshData.vertexBuffer->getContentDescription(),
                    sortedDrawCommands[0].meshData.elementBuffer->getContentDescription()
                );
            }

            const auto [mvpMatrixBufferSize, vertexBufferSize, elementBufferSize] = calcRequiredBufferSizes(sortedDrawCommands);
            resizeBuffersIfRequired(mvpMatrixBufferSize, vertexBufferSize, elementBufferSize);

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

            for (size_t drawCommandIndex = shaderGroup.firstIndex; drawCommandIndex <= shaderGroup.lastIndex; ++drawCommandIndex) {
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
                    shaderGroupRenderDataBuffers.mvpMatrixBuffer.get()->copyRange(sourceMVPMatrixBuffer, 0, mvpMatrixBufferBytesCopiedSoFar, sourceMVPMatrixBuffer.getSizeInBytes());

                    auto relativeBaseInstance = drawCommand.drawElementsIndirectCommand.baseInstance;
                    drawCommand.drawElementsIndirectCommand.baseInstance = relativeBaseInstance + mvpMatrixBufferBytesCopiedSoFar / sizeof(glm::mat4);

                    mvpMatrixBufferBytesCopiedSoFar += sourceMVPMatrixBuffer.getSizeInBytes();
                } else {
                    auto relativeBaseInstance = drawCommand.drawElementsIndirectCommand.baseInstance;
                    auto offsetWithinCombinedBuffer = mvpMatrixBufferBytesCopiedSoFar - sourceMVPMatrixBuffer.getSizeInBytes();
                    drawCommand.drawElementsIndirectCommand.baseInstance = relativeBaseInstance + offsetWithinCombinedBuffer / sizeof(glm::mat4);
                }

                if (hasNotCopiedThisVertexBuffer) {
                    vertexBufferToBaseVertex.insert_or_assign(drawCommand.meshData.vertexBuffer, baseVertexWithinNewBuffer);
                    shaderGroupRenderDataBuffers.vertexBuffer.get()->copyRange(sourceVertexBuffer, 0, vertexBufferBytesCopiedSoFar, sourceVertexBuffer.getSizeInBytes());
                    drawCommand.drawElementsIndirectCommand.baseVertex = baseVertexWithinNewBuffer + drawCommand.drawElementsIndirectCommand.baseVertex;

                    vertexBufferBytesCopiedSoFar += sourceVertexBuffer.getSizeInBytes();
                    baseVertexWithinNewBuffer += sourceVertexBuffer.getSizeInElements();
                } else {
                    auto meshStartWithinCombinedBuffer = vertexBufferSearchIterator->second;
                    drawCommand.drawElementsIndirectCommand.baseVertex = meshStartWithinCombinedBuffer + drawCommand.drawElementsIndirectCommand.baseVertex;
                }

                if (hasNotCopiedThisElementBuffer) {
                    elementBufferToFirstIndex.insert_or_assign(drawCommand.meshData.elementBuffer, firstIndexWithinNewBuffer);
                    shaderGroupRenderDataBuffers.elementBuffer.get()->copyRange(sourceElementBuffer, 0, elementBufferBytesCopiedSoFar, sourceElementBuffer.getSizeInBytes());
                    drawCommand.drawElementsIndirectCommand.firstIndex = firstIndexWithinNewBuffer + drawCommand.drawElementsIndirectCommand.firstIndex;

                    elementBufferBytesCopiedSoFar += sourceElementBuffer.getSizeInBytes();
                    firstIndexWithinNewBuffer += sourceElementBuffer.getSizeInElements();
                } else {
                    auto indicesStartWithinCombinedBuffer = elementBufferSearchIterator->second;
                    drawCommand.drawElementsIndirectCommand.firstIndex = indicesStartWithinCombinedBuffer + drawCommand.drawElementsIndirectCommand.firstIndex;
                }
            }
        }

        void IndirectRenderedSceneGraph::fillIndirectRenderCommandsBuffer(const SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup)
        {
            auto numDrawCommandsInShaderGroup = (shaderGroup.lastIndex - shaderGroup.firstIndex) + 1;

            //Guarantee the indirect commands GPU buffer is at least large enough to fit the current command set
            if (indirectRenderCommandsBuffer.get()->getSizeInElements() < numDrawCommandsInShaderGroup) {
                indirectRenderCommandsBuffer.resizeElements(indirectRenderCommandsBuffer.get()->getSizeInElements() * 2);
            }

            //Fill indirect render commands GPU buffer
            auto indirectRenderCommands = std::vector<RenderSystem::DrawElementsIndirectCommand>{};
            indirectRenderCommands.reserve(numDrawCommandsInShaderGroup);

            for (size_t i = shaderGroup.firstIndex; i <= shaderGroup.lastIndex; ++i) {
                indirectRenderCommands.push_back(sortedDrawCommands[i].drawElementsIndirectCommand);
            }

            indirectRenderCommandsBuffer.get()->bufferRange(
                0,
                indirectRenderCommands.size() * sizeof(RenderSystem::DrawElementsIndirectCommand),
                indirectRenderCommands.data()
            );
            indirectRenderCommandsBuffer.get()->bind(RenderSystem::RS_DRAW_INDIRECT_BUFFER);
        }

        void IndirectRenderedSceneGraph::combineBuffersAndMultiDrawEachMaterialGroup(SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup)
        {
            fillShaderGroupRenderDataBuffers(sortedDrawCommands, shaderGroup);
            fillIndirectRenderCommandsBuffer(sortedDrawCommands, shaderGroup);

            const auto &shaderDataForGroup = sortedDrawCommands[shaderGroup.firstIndex].shaderData;
            shaderDataForGroup.shader->bind();
            shaderDataForGroup.vertexArray->bind();

            shaderGroupRenderDataBuffers.elementBuffer.get()->bind(RenderSystem::RS_ELEMENT_ARRAY_BUFFER);
            shaderDataForGroup.vertexArray->linkBuffers(
                std::vector<std::reference_wrapper<const RenderSystem::Buffer>>{
                    *shaderGroupRenderDataBuffers.vertexBuffer.get(),
                    *shaderGroupRenderDataBuffers.mvpMatrixBuffer.get()
                }
            );

            for (const auto &materialGroup : shaderGroup.materialGroups) {
                bindMaterialTextures(*shaderDataForGroup.shader, sortedDrawCommands[materialGroup.firstIndex].textures);

                glMultiDrawElementsIndirect(
                    RenderSystem::RS_TRIANGLES,
                    RenderSystem::RS_UNSIGNED_INT,
                    0,
                    (materialGroup.lastIndex - materialGroup.firstIndex) + 1,
                    0
                );
            }
        }

        void IndirectRenderedSceneGraph::linkUserProvidedShaderBuffersAndRenderDrawCommandsIndividually(SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup)
        {
            fillIndirectRenderCommandsBuffer(sortedDrawCommands, shaderGroup);

            const auto &shaderDataForGroup = sortedDrawCommands[shaderGroup.firstIndex].shaderData;
            shaderDataForGroup.shader->bind();
            shaderDataForGroup.vertexArray->bind();

            shaderGroupRenderDataBuffers.elementBuffer.get()->bind(RenderSystem::RS_ELEMENT_ARRAY_BUFFER);

            for (const auto &materialGroup : shaderGroup.materialGroups) {
                bindMaterialTextures(*shaderDataForGroup.shader, sortedDrawCommands[materialGroup.firstIndex].textures);

                for (size_t drawCommandIndex = materialGroup.firstIndex; drawCommandIndex <= materialGroup.lastIndex; ++drawCommandIndex) {
                    auto &userProvidedShaderBuffers = *sortedDrawCommands[drawCommandIndex].meshData.userProvidedShaderBuffers;
                    auto buffersToLink = std::vector<std::reference_wrapper<const RenderSystem::Buffer>>{
                        *shaderGroupRenderDataBuffers.vertexBuffer.get(),
                        *shaderGroupRenderDataBuffers.mvpMatrixBuffer.get()
                    };
                    for (const auto buffer : userProvidedShaderBuffers) {
                        buffersToLink.push_back(*buffer);
                    }
                    shaderDataForGroup.vertexArray->linkBuffers(buffersToLink);

                    glMultiDrawElementsIndirect(
                        RenderSystem::RS_TRIANGLES,
                        RenderSystem::RS_UNSIGNED_INT,
                        reinterpret_cast<void *>(drawCommandIndex * sizeof(RenderSystem::DrawElementsIndirectCommand)),
                        1,
                        0
                    );
                }
            }
        }
    }
}
