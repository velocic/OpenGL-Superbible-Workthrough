#ifndef FLARE_SCENEGRAPH_INDIRECTRENDEREDSCENEGRAPH_H
#define FLARE_SCENEGRAPH_INDIRECTRENDEREDSCENEGRAPH_H

#include <memory>

#include <flare/rendersystem/texturemanager.h>
#include <flare/rendersystem/resizablebuffer.h>
#include <flare/scenegraph/scenegraph.h>
#include <flare/scenegraph/mesh.h>

namespace Flare
{
    namespace SceneGraph
    {
        class IndirectRenderedSceneGraph : public SceneGraph
        {
            private:
                using MaterialTextures = std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, std::nullptr_t>;
                using SortableDrawCommands = std::vector<Mesh::SortableDrawElementsIndirectCommand>;

                struct ShaderGroup;
                struct MaterialGroup;
                struct BufferGroup;

                struct ShaderGroup {
                    size_t firstIndex = 0;
                    size_t lastIndex = 0;
                    std::vector<MaterialGroup> materialGroups;
                };

                struct MaterialGroup {
                    size_t firstIndex = 0;
                    size_t lastIndex = 0;
                    std::vector<BufferGroup> bufferGroups;
                };

                struct BufferGroup {
                    size_t firstIndex = 0;
                    size_t lastIndex = 0;
                };

                struct ShaderGroupRenderDataBuffers {
                    RenderSystem::ResizableBuffer mvpMatrixBuffer;
                    RenderSystem::ResizableBuffer vertexBuffer;
                    RenderSystem::ResizableBuffer elementBuffer;
                    bool buffersHaveBeenInitialized = false;
                };

                ShaderGroupRenderDataBuffers shaderGroupRenderDataBuffers;
                RenderSystem::ResizableBuffer indirectRenderCommandsBuffer;
                std::unique_ptr<Node> rootNode;
                size_t nextNameToAssign = 0;

                size_t getMaterialId(const MaterialTextures &textures) const;
                void bindMaterialTextures(RenderSystem::ShaderProgram &shader, const MaterialTextures &textures);
                std::vector<ShaderGroup> sortDrawCommandsByShader(SortableDrawCommands &unsortedDrawCommands);

                void sortDrawCommandsWithinShaderGroupByMaterial(SortableDrawCommands &unsortedDrawCommands, ShaderGroup &shaderGroup);
                void sortDrawCommandsWithinMaterialGroupByMVPMatrixBuffer(SortableDrawCommands &unsortedDrawCommands, MaterialGroup &materialGroup);
                std::vector<MaterialGroup> getMaterialGroups(const SortableDrawCommands &drawCommandsSortedByMaterial, const ShaderGroup &shaderGroup);
                std::vector<BufferGroup> getBufferGroups(const SortableDrawCommands &drawCommandsSortedByMVPMatrixBuffer, const MaterialGroup &materialGroup);
                void initializeShaderGroupRenderDataBuffers(const RenderSystem::VertexDataLayout &mvpMatrixBufferLayout, const RenderSystem::VertexDataLayout &vertexBufferLayout, const RenderSystem::VertexDataLayout &elementBufferLayout);
                void fillShaderGroupRenderDataBuffers(SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup);
                void fillIndirectRenderCommandsBuffer(const SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup);
                void combineBuffersAndMultiDrawEachMaterialGroup(SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup);
                void linkUserProvidedShaderBuffersAndRenderDrawCommandsIndividually(SortableDrawCommands &sortedDrawCommands, const ShaderGroup &shaderGroup);

            public:
                IndirectRenderedSceneGraph();
                virtual ~IndirectRenderedSceneGraph();
                IndirectRenderedSceneGraph(IndirectRenderedSceneGraph &&other);
                IndirectRenderedSceneGraph &operator=(IndirectRenderedSceneGraph &&other);
                IndirectRenderedSceneGraph(const IndirectRenderedSceneGraph &other) = delete;
                IndirectRenderedSceneGraph &operator=(const IndirectRenderedSceneGraph &other) = delete;

                virtual void destroy() override;
                virtual Node *getRootNode() const override;
                virtual void render() override;
                virtual size_t requestName() override;
        };
    }
}

#endif
