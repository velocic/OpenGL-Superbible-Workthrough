#ifndef FLARE_SCENEGRAPH_MESH_H
#define FLARE_SCENEGRAPH_MESH_H

#include <flare/rendersystem/texturemanager.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/buffer.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Mesh
        {
            public:
                struct BoundData {
                    RenderSystem::ShaderData shaderData;
                    const RenderSystem::Buffer *mvpMatrixBuffer = nullptr;
                };

                struct MeshRenderData {
                    const RenderSystem::Buffer *mvpMatrixBuffer = nullptr;
                    const RenderSystem::Buffer *vertexBuffer = nullptr;
                    RenderSystem::Buffer *elementBuffer = nullptr;
                    size_t meshId = 0;
                };

                struct SortableDrawElementsIndirectCommand {
                    std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, std::nullptr_t> textures;
                    RenderSystem::DrawElementsIndirectCommand drawElementsIndirectCommand;
                    RenderSystem::ShaderData shaderData;
                    MeshRenderData meshData;
                };

                virtual ~Mesh() {}
                virtual void destroy() = 0;
                virtual void bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer) = 0;
                virtual size_t getName() const = 0;
                virtual void render(size_t instanceCount) = 0;
                virtual std::vector<SortableDrawElementsIndirectCommand> getIndirectDrawCommands(size_t instanceCount) const = 0;
        };
    }
}

#endif
