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
            }

            // sortDrawCommandsByMaterial(sortedDrawCommands);
            // auto drawCommandGroups = getMaterialGroups(sortedDrawCommands);
            // auto commandGroupQueue = std::queue<CommandGroupRange>(std::deque<CommandGroupRange>(commandGroupRanges.begin(), commandGroupRanges.end()));

            // // Sort each group of related render commands so that each command that shares a set of buffers is adjacent
            // for (size_t currentRange = 0; currentRange < drawCommandGroups.size(); ++currentRange) {
            //     const auto &range = drawCommandGroups[currentRange];
            //
            //     sortDrawCommandRangeByMVPMatrixBuffer(sortedDrawCommands.begin() + range.first, sortedDrawCommands.begin() + range.second + 1);
            // }

            // auto combinedRenderDataBuffers = getCombinedRenderDataBuffers(sortedDrawCommands, commandGroupRanges);
            //
            // //Guarantee the indirect commands GPU buffer is at least large enough to fit the current command set
            // if (indirectRenderCommandsBuffer.get()->getSizeInElements() < sortedDrawCommands.size()) {
            //     indirectRenderCommandsBuffer.resizeElements(indirectRenderCommandsBuffer.get()->getSizeInElements() * 2);
            // }
            //
            // //Fill indirect render commands GPU buffer
            // auto indirectRenderCommands = std::vector<RenderSystem::DrawElementsIndirectCommand>{};
            // indirectRenderCommands.reserve(sortedDrawCommands.size());
            // for (const auto &drawCommand : sortedDrawCommands) {
            //     indirectRenderCommands.push_back(drawCommand.drawElementsIndirectCommand);
            // }
            // indirectRenderCommandsBuffer.get()->bufferRange(
            //     0,
            //     indirectRenderCommands.size() * sizeof(RenderSystem::DrawElementsIndirectCommand),
            //     indirectRenderCommands.data()
            // );
            // indirectRenderCommandsBuffer.get()->bind(RenderSystem::RS_DRAW_INDIRECT_BUFFER);
            //
            // //Submit the indirect render commands
            // while (!commandGroupQueue.empty()) {
            //     const auto &drawCommandGroup = commandGroupQueue.front();
            //     const auto &firstDrawCommandInGroup = sortedDrawCommands[drawCommandGroup.first];
            //
            //     //Bind shader & material for draw command group
            //     firstDrawCommandInGroup.shaderData.shader->bind();
            //     firstDrawCommandInGroup.shaderData.vertexArray->bind();
            //     bindMaterialTextures(*firstDrawCommandInGroup.shaderData.shader, firstDrawCommandInGroup.textures);
            //
            //     combinedRenderDataBuffers.elementBuffer->bind(RenderSystem::RS_ELEMENT_ARRAY_BUFFER);
            //     firstDrawCommandInGroup.shaderData.vertexArray->linkBuffers(
            //         std::vector<std::reference_wrapper<const RenderSystem::Buffer>>{
            //             *combinedRenderDataBuffers.vertexBuffer,
            //             *combinedRenderDataBuffers.mvpMatrixBuffer
            //         }
            //     );
            //
            //     glMultiDrawElementsIndirect(
            //         RenderSystem::RS_TRIANGLES,
            //         RenderSystem::RS_UNSIGNED_INT,
            //         reinterpret_cast<void *>(drawCommandGroup.first * sizeof(RenderSystem::DrawElementsIndirectCommand)),
            //         (drawCommandGroup.second - drawCommandGroup.first) + 1,//number of draw commands in the group
            //         0
            //     );
            //
            //     commandGroupQueue.pop();
            // }
        }

        size_t IndirectRenderedSceneGraph::requestName()
        {
            return nextNameToAssign++;
        }

        size_t IndirectRenderedSceneGraph::getMaterialId(const MaterialTextures &textures)
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

        void IndirectRenderedSceneGraph::sortDrawCommandRangeByMVPMatrixBuffer(SortableDrawCommands::iterator begin, SortableDrawCommands::iterator end)
        {
            std::sort(
                begin,
                end,
                [](const auto &command1, const auto &command2){
                    return command1.meshData.mvpMatrixBuffer->getName() == command2.meshData.mvpMatrixBuffer->getName();
                }
            );
        }

        std::vector<IndirectRenderedSceneGraph::MaterialGroup> IndirectRenderedSceneGraph::getMaterialGroups(const SortableDrawCommands &drawCommandsSortedByMaterial, const ShaderGroup &shaderGroup)
        {
            auto lastMaterialGroupIndexStart = shaderGroup.firstIndex;
            auto materialGroups = std::vector<MaterialGroup>{};

            auto numDrawCommandsInShaderGroup = (shaderGroup.lastIndex - shaderGroup.firstIndex) + 1;

            if (numDrawCommandsInShaderGroup == 1) {
                materialGroups.push_back(MaterialGroup{shaderGroup.firstIndex, shaderGroup.firstIndex});
                return materialGroups;
            }

            for (size_t i = shaderGroup.firstIndex; i <= shaderGroup.lastIndex; ++i) {
                const auto &startOfRangeDrawCommand = drawCommandsSortedByMaterial[lastMaterialGroupIndexStart];
                const auto &currentDrawCommand = drawCommandsSortedByMaterial[i];
                auto isLastIteration = i == shaderGroup.lastIndex;

                if ((currentDrawCommand.shaderData.hashedAlias != startOfRangeDrawCommand.shaderData.hashedAlias)
                    || (getMaterialId(currentDrawCommand.textures) != getMaterialId(startOfRangeDrawCommand.textures))
                    || isLastIteration) {

                    if (isLastIteration) {
                        materialGroups.push_back(MaterialGroup{lastMaterialGroupIndexStart, i});
                    } else {
                        materialGroups.push_back(MaterialGroup{lastMaterialGroupIndexStart, i - 1});
                    }

                    lastMaterialGroupIndexStart = i;
                }
            }

            return materialGroups;
        }

        // IndirectRenderedSceneGraph::CombinedRenderDataBuffers IndirectRenderedSceneGraph::getCombinedRenderDataBuffers(SortableDrawCommands &sortedDrawCommands, const std::vector<ShaderGroup> &shaderGroupRanges)
        // {
        //     auto combinedBuffers = CombinedRenderDataBuffers{};
        //     combinedBuffers.mvpMatrixBuffer = RenderSystem::createBuffer("mvpMatrix", sortedDrawCommands[0].meshData.mvpMatrixBuffer->getContentDescription());
        //     combinedBuffers.vertexBuffer = RenderSystem::createBuffer("vertexBuffer", sortedDrawCommands[0].meshData.vertexBuffer->getContentDescription());
        //     combinedBuffers.elementBuffer = RenderSystem::createBuffer("elementBuffer", sortedDrawCommands[0].meshData.elementBuffer->getContentDescription());
        //
        //     //calc required size for each buffer, then allocate storage
        //     auto combinedMVPMatrixBufferSize = size_t{0};
        //     auto combinedVertexBufferSize = size_t{0};
        //     auto combinedElementBufferSize = size_t{0};
        //
        //     {
        //         auto mvpMatrixBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
        //         auto vertexBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
        //         auto elementBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
        //
        //         for (const auto &drawCommand : sortedDrawCommands) {
        //             auto hasCountedThisMVPMatrixBuffer = mvpMatrixBuffersEncountered.find(drawCommand.meshData.mvpMatrixBuffer) != mvpMatrixBuffersEncountered.end();
        //             auto hasCountedThisVertexBuffer = vertexBuffersEncountered.find(drawCommand.meshData.vertexBuffer) != vertexBuffersEncountered.end();
        //             auto hasCountedThisElementBuffer = elementBuffersEncountered.find(drawCommand.meshData.elementBuffer) != elementBuffersEncountered.end();
        //
        //             if (!hasCountedThisMVPMatrixBuffer) {
        //                 combinedMVPMatrixBufferSize += drawCommand.meshData.mvpMatrixBuffer->getSizeInBytes();
        //                 mvpMatrixBuffersEncountered.insert(drawCommand.meshData.mvpMatrixBuffer);
        //             }
        //
        //             if (!hasCountedThisVertexBuffer) {
        //                 combinedVertexBufferSize += drawCommand.meshData.vertexBuffer->getSizeInBytes();
        //                 vertexBuffersEncountered.insert(drawCommand.meshData.vertexBuffer);
        //             }
        //
        //             if (!hasCountedThisElementBuffer) {
        //                 combinedElementBufferSize += drawCommand.meshData.elementBuffer->getSizeInBytes();
        //                 elementBuffersEncountered.insert(drawCommand.meshData.elementBuffer);
        //             }
        //         }
        //     }
        //
        //     combinedBuffers.mvpMatrixBuffer->allocateBufferStorage(combinedMVPMatrixBufferSize, nullptr, 0);
        //     combinedBuffers.vertexBuffer->allocateBufferStorage(combinedVertexBufferSize, nullptr, 0);
        //     combinedBuffers.elementBuffer->allocateBufferStorage(combinedElementBufferSize, nullptr, 0);
        //
        //     //copy separated buffers into combined buffers, and update DrawElementsIndirectCommand(s) to point to the
        //     //correct offsets within the new buffers
        //     auto mvpMatrixBufferBytesCopiedSoFar = size_t{0};
        //     auto vertexBufferBytesCopiedSoFar = size_t{0};
        //     auto elementBufferBytesCopiedSoFar = size_t{0};
        //
        //     auto baseVertexWithinNewBuffer = size_t{0};
        //     auto firstIndexWithinNewBuffer = size_t{0};
        //
        //     auto mvpMatrixBuffersEncountered = std::unordered_set<const RenderSystem::Buffer *>{};
        //     auto vertexBufferToBaseVertex = std::unordered_map<const RenderSystem::Buffer *, size_t>{};
        //     auto elementBufferToFirstIndex = std::unordered_map<const RenderSystem::Buffer *, size_t>{};
        //
        //     for (size_t currentRange = 0; currentRange < commandGroupRanges.size(); ++currentRange) {
        //         const auto &commandGroupRange = commandGroupRanges[currentRange];
        //
        //         for (size_t drawCommandIndex = commandGroupRange.first; drawCommandIndex < commandGroupRange.second + 1; ++drawCommandIndex) {
        //             auto &drawCommand = sortedDrawCommands[drawCommandIndex];
        //
        //             const auto &sourceMVPMatrixBuffer = *drawCommand.meshData.mvpMatrixBuffer;
        //             const auto &sourceVertexBuffer = *drawCommand.meshData.vertexBuffer;
        //             const auto &sourceElementBuffer = *drawCommand.meshData.elementBuffer;
        //
        //             auto mvpMatrixBufferSearchIterator = mvpMatrixBuffersEncountered.find(drawCommand.meshData.mvpMatrixBuffer);
        //             auto vertexBufferSearchIterator = vertexBufferToBaseVertex.find(drawCommand.meshData.vertexBuffer);
        //             auto elementBufferSearchIterator = elementBufferToFirstIndex.find(drawCommand.meshData.elementBuffer);
        //
        //             auto hasNotCopiedThisMVPMatrixBuffer = mvpMatrixBufferSearchIterator == mvpMatrixBuffersEncountered.end();
        //             auto hasNotCopiedThisVertexBuffer = vertexBufferSearchIterator == vertexBufferToBaseVertex.end();
        //             auto hasNotCopiedThisElementBuffer = elementBufferSearchIterator == elementBufferToFirstIndex.end();
        //
        //             if (hasNotCopiedThisMVPMatrixBuffer) {
        //                 mvpMatrixBuffersEncountered.insert(drawCommand.meshData.mvpMatrixBuffer);
        //                 combinedBuffers.mvpMatrixBuffer->copyRange(sourceMVPMatrixBuffer, 0, mvpMatrixBufferBytesCopiedSoFar, sourceMVPMatrixBuffer.getSizeInBytes());
        //
        //                 drawCommand.drawElementsIndirectCommand.baseInstance = mvpMatrixBufferBytesCopiedSoFar / sizeof(glm::mat4);
        //
        //                 mvpMatrixBufferBytesCopiedSoFar += sourceMVPMatrixBuffer.getSizeInBytes();
        //             } else {
        //                 auto relativeBaseInstance = drawCommand.drawElementsIndirectCommand.baseInstance;
        //                 auto offsetWithinCombinedBuffer = mvpMatrixBufferBytesCopiedSoFar - sourceMVPMatrixBuffer.getSizeInBytes();
        //                 drawCommand.drawElementsIndirectCommand.baseInstance = relativeBaseInstance + offsetWithinCombinedBuffer / sizeof(glm::mat4);
        //             }
        //
        //             if (hasNotCopiedThisVertexBuffer) {
        //                 vertexBufferToBaseVertex.insert_or_assign(drawCommand.meshData.vertexBuffer, baseVertexWithinNewBuffer);
        //                 combinedBuffers.vertexBuffer->copyRange(sourceVertexBuffer, 0, vertexBufferBytesCopiedSoFar, sourceVertexBuffer.getSizeInBytes());
        //                 drawCommand.drawElementsIndirectCommand.baseVertex = baseVertexWithinNewBuffer + drawCommand.drawElementsIndirectCommand.baseVertex;
        //
        //                 vertexBufferBytesCopiedSoFar += sourceVertexBuffer.getSizeInBytes();
        //                 baseVertexWithinNewBuffer += sourceVertexBuffer.getSizeInElements();
        //             } else {
        //                 auto meshStartWithinCombinedBuffer = vertexBufferSearchIterator->second;
        //                 drawCommand.drawElementsIndirectCommand.baseVertex = meshStartWithinCombinedBuffer + drawCommand.drawElementsIndirectCommand.baseVertex;
        //             }
        //
        //             if (hasNotCopiedThisElementBuffer) {
        //                 elementBufferToFirstIndex.insert_or_assign(drawCommand.meshData.elementBuffer, firstIndexWithinNewBuffer);
        //                 combinedBuffers.elementBuffer->copyRange(sourceElementBuffer, 0, elementBufferBytesCopiedSoFar, sourceElementBuffer.getSizeInBytes());
        //                 drawCommand.drawElementsIndirectCommand.firstIndex = firstIndexWithinNewBuffer + drawCommand.drawElementsIndirectCommand.firstIndex;
        //
        //                 elementBufferBytesCopiedSoFar += sourceElementBuffer.getSizeInBytes();
        //                 firstIndexWithinNewBuffer += sourceElementBuffer.getSizeInElements();
        //             } else {
        //                 auto indicesStartWithinCombinedBuffer = elementBufferSearchIterator->second;
        //                 drawCommand.drawElementsIndirectCommand.firstIndex = indicesStartWithinCombinedBuffer + drawCommand.drawElementsIndirectCommand.firstIndex;
        //             }
        //         }
        //     }
        //
        //     return combinedBuffers;
        // }
    }
}
