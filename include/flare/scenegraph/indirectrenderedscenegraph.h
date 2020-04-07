#ifndef FLARE_SCENEGRAPH_INDIRECTRENDEREDSCENEGRAPH_H
#define FLARE_SCENEGRAPH_INDIRECTRENDEREDSCENEGRAPH_H

#include <memory>

#include <flare/rendersystem/resizablebuffer.h>
#include <flare/scenegraph/scenegraph.h>

namespace Flare
{
    namespace SceneGraph
    {
        class IndirectRenderedSceneGraph : public SceneGraph
        {
            private:
                struct CombinedRenderDataBuffers {
                    std::unique_ptr<RenderSystem::Buffer> mvpMatrixBuffer;
                    std::unique_ptr<RenderSystem::Buffer> vertexBuffer;
                    std::unique_ptr<RenderSystem::Buffer> elementBuffer;
                };

                using MaterialTextures = std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, std::nullptr_t>;
                using SortableDrawCommands = std::vector<Mesh::SortableDrawElementsIndirectCommand>;
                using CommandGroupRanges = std::vector<std::pair<size_t, size_t>>{};

                RenderSystem::ResizableBuffer indirectRenderCommandsBuffer;
                std::unique_ptr<Node> rootNode;
                size_t nextNameToAssign = 0;

                size_t getMaterialId(const MaterialTextures &textures);
                void bindMaterialTextures(RenderSystem::ShaderProgram &shader, const Material &textures);
                void sortDrawCommandsByMaterial(SortableDrawCommands &unsortedDrawCommands);
                void sortDrawCommandRangeByMVPMatrixBuffer(SortableDrawCommands::iterator begin, SortableDrawCommands::iterator end);
                CommandGroupRanges getCommandGroupRanges(const SortableDrawCommands &drawCommandsSortedByMaterial);
                CombinedRenderDataBuffers getCombinedRenderDataBuffers(SortableDrawCommands &sortedDrawCommands, const CommandGroupRanges &commandGroupRanges);

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
