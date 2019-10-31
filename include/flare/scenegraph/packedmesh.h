#ifndef FLARE_SCENEGRAPH_PACKEDMESH_H
#define FLARE_SCENEGRAPH_PACKEDMESH_H

#include <memory>
#include <variant>
#include <vector>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/datatypes.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/mesh.h>
#include <flare/utility/datatypes.h>

namespace Flare
{
    namespace SceneGraph
    {
        class PackedMesh : public Mesh
        {
            public:
                struct SubMeshEntry;
            private:
                std::vector<SubMeshEntry> subMeshEntries;
                std::unique_ptr<RenderSystem::Buffer> VBO;
                std::unique_ptr<RenderSystem::Buffer> EBO;
                BoundData boundData;
                size_t name = 0;

                void populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
            public:
                struct SubMeshEntry {
                    std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, std::nullptr_t> textures;
                    glm::mat4 localTransform;
                    RenderSystem::RSsizei elementCount;
                    RenderSystem::RSint baseVertex;
                    size_t elementBufferOffset = 0;
                };

                PackedMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::vector<SubMeshEntry> &&subMeshEntries);
                virtual ~PackedMesh();
                PackedMesh(PackedMesh &&other);
                PackedMesh &operator=(PackedMesh &&other);
                PackedMesh(const PackedMesh &other) = delete;
                PackedMesh &operator=(const PackedMesh &other) = delete;

                virtual void destroy() override;
                virtual void bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer) override;
                virtual std::vector<glm::mat4> getLocalTransforms() const override;
                virtual size_t getName() const override;
                virtual size_t getPackedMeshCount() const override { return subMeshEntries.size(); }
                virtual void render(size_t instanceCount, size_t) override;
                virtual std::vector<SortableDrawElementsIndirectCommand> getIndirectDrawCommands(size_t instanceCount, size_t) const override;
        };
    }
}

#endif
