#include <flare/scenegraph/packedmesh.h>

#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace SceneGraph
    {
        PackedMesh::PackedMesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::vector<SubMeshEntry> &&subMeshEntries)
        :
            subMeshEntries(std::move(subMeshEntries))
        {
            populateBuffers(std::move(vertices), std::move(indices));
        }

        PackedMesh::~PackedMesh()
        {
            destroy();
        }

        PackedMesh::PackedMesh(PackedMesh &&other)
        :
            subMeshEntries(std::move(other.subMeshEntries)),
            VBO(std::move(other.VBO)),
            EBO(std::move(other.EBO))
        {
        }

        PackedMesh &PackedMesh::operator=(PackedMesh &&other)
        {
            subMeshEntries = std::move(other.subMeshEntries);
            VBO = std::move(other.VBO);
            EBO = std::move(other.EBO);

            return *this;
        }

        void PackedMesh::destroy()
        {
            VBO = nullptr;
            EBO = nullptr;
            std::vector<SubMeshEntry>().swap(subMeshEntries);
        }

        void PackedMesh::bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer)
        {
            boundData.shaderData = shaderData;
            boundData.mvpMatrixBuffer = &mvpMatrixBuffer;
        }

        void PackedMesh::render(size_t instanceCount)
        {
            if (boundData.shaderData.shader == nullptr
                || boundData.shaderData.vertexArray == nullptr
                || boundData.mvpMatrixBuffer == nullptr) {
                throw std::runtime_error("Packed mesh has not been bound; Invalid render operation.");
            }

            boundData.shaderData.vertexArray->linkBuffers(
                std::vector<std::reference_wrapper<const RenderSystem::Buffer>>{*VBO, *boundData.mvpMatrixBuffer}
            );
            boundData.shaderData.shader->bind();
            boundData.shaderData.vertexArray->bind();
            EBO->bind(GL_ELEMENT_ARRAY_BUFFER);

            for (auto &subMeshEntry : subMeshEntries) {
                if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(subMeshEntry.textures)) {
                    const auto &phongTextures = std::get<RenderSystem::PhongMaterialTextures>(subMeshEntry.textures);
                    for (size_t i = 0; i < phongTextures.diffuse.size(); ++i) {
                        boundData.shaderData.shader->setTexture("textureDiffuse" + std::to_string(i), phongTextures.diffuse[i]);
                    }
                    for (size_t i = 0; i < phongTextures.specular.size(); ++i) {
                        boundData.shaderData.shader->setTexture("textureSpecular" + std::to_string(i), phongTextures.specular[i]);
                    }
                    for (size_t i = 0; i < phongTextures.normal.size(); ++i) {
                        boundData.shaderData.shader->setTexture("textureNormal" + std::to_string(i), phongTextures.normal[i]);
                    }
                } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(subMeshEntry.textures)) {
                    const auto &PBRTextures = std::get<RenderSystem::PBRMaterialTextures>(subMeshEntry.textures);
                    for (size_t i = 0; i < PBRTextures.baseColor.size(); ++i) {
                        boundData.shaderData.shader->setTexture("textureBaseColor" + std::to_string(i), PBRTextures.baseColor[i]);
                    }
                    for (size_t i = 0; i < PBRTextures.normal.size(); ++i) {
                        boundData.shaderData.shader->setTexture("textureNormal" + std::to_string(i), PBRTextures.normal[i]);
                    }
                    for (size_t i = 0; i < PBRTextures.metallic.size(); ++i) {
                        boundData.shaderData.shader->setTexture("textureMetallic" + std::to_string(i), PBRTextures.metallic[i]);
                    }
                    for (size_t i = 0; i < PBRTextures.roughness.size(); ++i) {
                        boundData.shaderData.shader->setTexture("textureRoughness" + std::to_string(i), PBRTextures.roughness[i]);
                    }
                }

                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    subMeshEntry.elementCount,
                    GL_UNSIGNED_INT,
                    0,
                    instanceCount,
                    subMeshEntry.baseVertex,
                    0
                );
            }
        }

        void PackedMesh::populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices)
        {
            auto vertexBufferLayout = RenderSystem::VertexDataLayoutBuilder()
                .addAttribute("position", 3, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, 0)
                .addAttribute("normal", 3, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(glm::vec3))
                .addAttribute("uvCoord", 2, RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(glm::vec3) * 2)
                .setStride(sizeof(DataTypes::Vertex))
                .build();

            auto elementBufferLayout = RenderSystem::VertexDataLayoutBuilder()
                .addAttribute("element", 1, RenderSystem::RS_UNSIGNED_INT, RenderSystem::RS_FALSE, 0)
                .setStride(sizeof(unsigned int))
                .build();

            VBO = RenderSystem::createBuffer("vertexBuffer", vertexBufferLayout);
            VBO->allocateBufferStorage(
                vertices.size() * sizeof(DataTypes::Vertex),
                vertices.data(),
                0
            );

            EBO = RenderSystem::createBuffer("elementBuffer", elementBufferLayout);
            EBO->allocateBufferStorage(
                indices.size() * sizeof(unsigned int),
                indices.data(),
                0
            );
        }
    }
}
