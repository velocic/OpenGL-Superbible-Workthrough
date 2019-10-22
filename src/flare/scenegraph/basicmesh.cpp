#include <flare/scenegraph/basicmesh.h>

#include <flare/rendersystem/datalayout.h>
#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace SceneGraph
    {
        BasicMesh::BasicMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PhongMaterialTextures &textures)
        :
            textures(textures),
            elementCount(indices.size()),
            name(name)
        {
            populateBuffers(
                std::move(vertices),
                std::move(indices)
            );
        }

        BasicMesh::BasicMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PBRMaterialTextures &textures)
        :
            textures(textures),
            elementCount(indices.size()),
            name(name)
        {
            populateBuffers(
                std::move(vertices),
                std::move(indices)
            );
        }

        BasicMesh::BasicMesh(size_t name, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices)
        :
            textures(nullptr),
            elementCount(indices.size()),
            name(name)
        {
            populateBuffers(
                std::move(vertices),
                std::move(indices)
            );
        }

        BasicMesh::~BasicMesh()
        {
            destroy();
        }

        BasicMesh::BasicMesh(BasicMesh &&other)
        :
            textures(std::move(other.textures)),
            VBO(std::move(other.VBO)),
            EBO(std::move(other.EBO)),
            name(std::exchange(other.name, 0)),
            elementCount(std::exchange(other.elementCount, 0))
        {
        }

        BasicMesh &BasicMesh::operator=(BasicMesh &&other)
        {
            textures = std::move(other.textures);
            VBO = std::move(other.VBO);
            EBO = std::move(other.EBO);
            name = std::exchange(other.name, 0);
            elementCount = std::exchange(other.elementCount, 0);

            return *this;
        }

        void BasicMesh::populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices)
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

        void BasicMesh::destroy()
        {
            VBO = nullptr;
            EBO = nullptr;
        }

        void BasicMesh::bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer)
        {
            boundData.shaderData = shaderData;
            boundData.mvpMatrixBuffer = &mvpMatrixBuffer;

            if (std::holds_alternative<RenderSystem::PhongMaterialTextures>(textures)) {
                const auto &phongTextures = std::get<RenderSystem::PhongMaterialTextures>(textures);
                for (size_t i = 0; i < phongTextures.diffuse.size(); ++i) {
                    shaderData.shader->setTexture("textureDiffuse" + std::to_string(i), phongTextures.diffuse[i]);
                }
                for (size_t i = 0; i < phongTextures.specular.size(); ++i) {
                    shaderData.shader->setTexture("textureSpecular" + std::to_string(i), phongTextures.specular[i]);
                }
                for (size_t i = 0; i < phongTextures.normal.size(); ++i) {
                    shaderData.shader->setTexture("textureNormal" + std::to_string(i), phongTextures.normal[i]);
                }
            } else if (std::holds_alternative<RenderSystem::PBRMaterialTextures>(textures)) {
                const auto &PBRTextures = std::get<RenderSystem::PBRMaterialTextures>(textures);
                for (size_t i = 0; i < PBRTextures.baseColor.size(); ++i) {
                    shaderData.shader->setTexture("textureBaseColor" + std::to_string(i), PBRTextures.baseColor[i]);
                }
                for (size_t i = 0; i < PBRTextures.normal.size(); ++i) {
                    shaderData.shader->setTexture("textureNormal" + std::to_string(i), PBRTextures.normal[i]);
                }
                for (size_t i = 0; i < PBRTextures.metallic.size(); ++i) {
                    shaderData.shader->setTexture("textureMetallic" + std::to_string(i), PBRTextures.metallic[i]);
                }
                for (size_t i = 0; i < PBRTextures.roughness.size(); ++i) {
                    shaderData.shader->setTexture("textureRoughness" + std::to_string(i), PBRTextures.roughness[i]);
                }
            }

            shaderData.vertexArray->linkBuffers(
                std::vector<std::reference_wrapper<const RenderSystem::Buffer>>{*VBO, mvpMatrixBuffer}
            );
            shaderData.shader->bind();
            shaderData.vertexArray->bind();
            EBO->bind(GL_ELEMENT_ARRAY_BUFFER);
        }

        size_t BasicMesh::getName() const
        {
            return name;
        }

        void BasicMesh::render(size_t instanceCount)
        {
            //TODO: abstract the GL call here into a platform-independent wrapper
            glDrawElementsInstanced(
                GL_TRIANGLES,
                elementCount,
                GL_UNSIGNED_INT,
                0, //offset into buffer containing elements (this buffer is elements only, so no offset necessary)
                instanceCount //size of MVP matrix buffer, handed down from SceneNode -> Model -> BasicMesh
            );
        }

        std::vector<Mesh::SortableDrawElementsIndirectCommand> BasicMesh::getIndirectDrawCommands(size_t instanceCount) const
        {
            auto result = std::vector<Mesh::SortableDrawElementsIndirectCommand>{};
            auto drawCommand = Mesh::SortableDrawElementsIndirectCommand{};
            drawCommand.textures = textures;
            drawCommand.shaderData = boundData.shaderData;
            drawCommand.drawElementsIndirectCommand = RenderSystem::DrawElementsIndirectCommand{
                static_cast<RenderSystem::RSuint>(elementCount),
                static_cast<RenderSystem::RSuint>(instanceCount),
                0,
                0,
                0
            };
            drawCommand.meshData = MeshRenderData{
                boundData.mvpMatrixBuffer,
                VBO.get(),
                EBO.get(),
                getName()
            };

            result.push_back(std::move(drawCommand));

            return result;
        }
    }
}
