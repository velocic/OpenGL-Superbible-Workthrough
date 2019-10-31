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
                glm::mat4 localTransform;
                BoundData boundData;
                size_t name = 0;
                size_t elementCount = 0;

                void populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
            public:
                BasicMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PhongMaterialTextures &textures, const glm::mat4 &localTransform);
                BasicMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PBRMaterialTextures &textures, const glm::mat4 &localTransform);
                BasicMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const glm::mat4 &localTransform);
                virtual ~BasicMesh();
                BasicMesh(BasicMesh &&other);
                BasicMesh &operator=(BasicMesh &&other);
                BasicMesh(const BasicMesh &other) = delete;
                BasicMesh &operator=(const BasicMesh &other) = delete;

                virtual void destroy() override;
                virtual void bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer) override;
                virtual std::vector<glm::mat4> getLocalTransforms() const override;
                virtual size_t getName() const override;
                virtual size_t getPackedMeshCount() const override { return 1; }
                virtual void render(size_t instanceCount, size_t subMeshIndex) override;
                virtual std::vector<SortableDrawElementsIndirectCommand> getIndirectDrawCommands(size_t instanceCount, size_t subMeshIndex) const override;
        };
    }
}

#endif
