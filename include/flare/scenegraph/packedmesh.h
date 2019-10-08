#ifndef FLARE_SCENEGRAPH_PACKEDMESH_H
#define FLARE_SCENEGRAPH_PACKEDMESH_H

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
        class PackedMesh : Mesh
        {
            private:
                std::vector<SubMeshEntry> subMeshEntries;
                std::unique_ptr<RenderSystem::Buffer> VBO;
                std::unique_ptr<RenderSystem::Buffer> EBO;

                void populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
            public:
                struct SubMeshEntry {
                    std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, nullptr_t> textures;
                    RSsizei elementCount;
                    RSint baseVertex;
                    RSuint baseInstance;
                };

                PackedMesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::vector<SubMeshEntry> &&subMeshEntries);
                ~PackedMesh();
                PackedMesh(PackedMesh &&other);
                PackedMesh &operator=(PackedMesh &&other);
                PackedMesh(const PackedMesh &other) = delete;
                PackedMesh &operator=(const PackedMesh &other) = delete;

                virtual void destroy() override;
                virtual void bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer) override;
                virtual void render(size_t instanceCount) override;
        };
    }
}

#endif
