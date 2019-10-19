#ifndef FLARE_SCENEGRAPH_BASICMESH_H
#define FLARE_SCENEGRAPH_BASICMESH_H

#include <memory>
#include <variant>
#include <vector>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/mesh.h>
#include <flare/utility/datatypes.h>

namespace Flare
{
    namespace SceneGraph
    {
        class BasicMesh : public Mesh
        {
            private:
                std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, std::nullptr_t> textures;
                std::unique_ptr<RenderSystem::Buffer> VBO;
                std::unique_ptr<RenderSystem::Buffer> EBO;
                BoundData boundData;
                size_t elementCount = 0;

                void populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
            public:
                BasicMesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PhongMaterialTextures &textures);
                BasicMesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PBRMaterialTextures &textures);
                BasicMesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
                virtual ~BasicMesh();
                BasicMesh(BasicMesh &&other);
                BasicMesh &operator=(BasicMesh &&other);
                BasicMesh(const BasicMesh &other) = delete;
                BasicMesh &operator=(const BasicMesh &other) = delete;

                virtual void destroy() override;
                virtual void bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer) override;
                virtual void render(size_t instanceCount) override;
                virtual std::vector<SortableDrawElementsIndirectCommand> getIndirectDrawCommands(size_t instanceCount) const override;
        };
    }
}

#endif
