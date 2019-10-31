#include <flare/scenegraph/packedmesh.h>

#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace SceneGraph
    {
        PackedMesh::PackedMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::vector<SubMeshEntry> &&subMeshEntries)
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
            EBO(std::move(other.EBO)),
            name(std::exchange(other.name, 0))
        {
        }

        PackedMesh &PackedMesh::operator=(PackedMesh &&other)
        {
            subMeshEntries = std::move(other.subMeshEntries);
            VBO = std::move(other.VBO);
            EBO = std::move(other.EBO);
            name = std::exchange(other.name, 0);

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

        std::vector<glm::mat4> PackedMesh::getLocalTransforms() const
        {
            auto result = std::vector<glm::mat4>{};

            for (const auto &subMeshEntry : subMeshEntries) {
                result.push_back(subMeshEntry.localTransform);
            }

            return result;
        }

        size_t PackedMesh::getName() const
        {
            return name;
        }

        void PackedMesh::render(size_t instanceCount, size_t)
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

            for (size_t subMeshIndex = 0; subMeshIndex < subMeshEntries.size(); ++subMeshIndex) {
                auto &subMeshEntry = subMeshEntries[subMeshIndex];

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

                //TODO: abstract the GL call here into a platform-independent wrapper
                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    subMeshEntry.elementCount,
                    GL_UNSIGNED_INT,
                    reinterpret_cast<void *>(subMeshEntry.elementBufferOffset),
                    instanceCount,
                    subMeshEntry.baseVertex,
                    instanceCount * subMeshIndex
                );
            }
        }

        std::vector<Mesh::SortableDrawElementsIndirectCommand> PackedMesh::getIndirectDrawCommands(size_t instanceCount, size_t) const
        {
            if (boundData.shaderData.shader == nullptr
                || boundData.shaderData.vertexArray == nullptr
                || boundData.mvpMatrixBuffer == nullptr) {
                throw std::runtime_error("Packed mesh has not been bound; Invalid render operation.");
            }

            auto result = std::vector<Mesh::SortableDrawElementsIndirectCommand>{};

            for (size_t subMeshIndex = 0; subMeshIndex < subMeshEntries.size(); ++subMeshIndex) {
                const auto &subMeshEntry = subMeshEntries[subMeshIndex];

                auto drawCommand = Mesh::SortableDrawElementsIndirectCommand{};
                drawCommand.textures = subMeshEntry.textures;
                drawCommand.shaderData = boundData.shaderData;

                //Note: DrawElementsIndirectCommand.firstIndex is calculated differently than elementBufferOffset
                //used in the direct render draw path. elementBufferOffset is numIndices * sizeof(RenderSystem::RSsizei), but
                //firstIndex is simply numIndices (which is why the division by sizeof(RenderSystem::RSsizei) happens below.
                drawCommand.drawElementsIndirectCommand = RenderSystem::DrawElementsIndirectCommand{
                    static_cast<RenderSystem::RSuint>(subMeshEntry.elementCount),
                    static_cast<RenderSystem::RSuint>(instanceCount),
                    static_cast<RenderSystem::RSuint>(subMeshEntry.elementBufferOffset / sizeof(RenderSystem::RSsizei)),
                    static_cast<RenderSystem::RSuint>(subMeshEntry.baseVertex),
                    static_cast<RenderSystem::RSuint>(instanceCount * subMeshIndex)
                };
                drawCommand.meshData = MeshRenderData{
                    boundData.mvpMatrixBuffer,
                    VBO.get(),
                    EBO.get(),
                    getName()
                };

                result.push_back(std::move(drawCommand));
            }

            return result;
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
