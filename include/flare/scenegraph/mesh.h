#ifndef FLARE_SCENEGRAPH_MESH_H
#define FLARE_SCENEGRAPH_MESH_H

#include <vector>

#include <flare/rendersystem/texturemanager.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/buffer.h>

#include <glm-0.9.9/glm.hpp>

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
                    std::vector<const RenderSystem::Buffer *> userProvidedShaderBuffers;
                };

                struct MeshRenderData {
                    const RenderSystem::Buffer *mvpMatrixBuffer = nullptr;
                    const RenderSystem::Buffer *vertexBuffer = nullptr;
                    RenderSystem::Buffer *elementBuffer = nullptr;
                    const std::vector<const RenderSystem::Buffer *> *userProvidedShaderBuffers = nullptr;
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
                virtual void bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer, const std::vector<const RenderSystem::Buffer *> &userProvidedShaderBuffers) = 0;
                virtual std::vector<glm::mat4> getLocalTransforms() const = 0;
                virtual size_t getName() const = 0;
                virtual size_t getPackedMeshCount() const = 0;
                virtual void render(size_t instanceCount, size_t subMeshIndex) = 0;
                virtual std::vector<SortableDrawElementsIndirectCommand> getIndirectDrawCommands(size_t instanceCount, size_t subMeshIndex) const = 0;
        };
    }
}

#endif
