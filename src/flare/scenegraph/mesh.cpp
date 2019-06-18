#include <flare/scenegraph/mesh.h>

#include <flare/rendersystem/datalayout.h>
#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace SceneGraph
    {
        Mesh::Mesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::vector<TextureUnitBinding> &&textures)
        :
            textures(textures),
            elementCount(indices.size())
        {
            populateBuffers(
                std::move(vertices),
                std::move(indices)
            );
        }

        Mesh::~Mesh()
        {
            destroy();
        }

        Mesh::Mesh(Mesh &&other)
        :
            textures(std::move(other.textures)),
            elementCount(other.elementCount)
        {
            other.elementCount = 0;
        }

        Mesh &Mesh::operator=(Mesh &&other)
        {
            textures = std::move(other.textures);
            elementCount = other.elementCount;

            other.elementCount = 0;

            return *this;
        }

        void Mesh::populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices)
        {
            auto vertexBufferLayout = RenderSystem::VertexDataLayoutBuilder()
                .addAttribute("position", sizeof(glm::vec3), RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, 0)
                .addAttribute("normal", sizeof(glm::vec3), RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(glm::vec3))
                .addAttribute("uvCoords", sizeof(glm::vec2), RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, sizeof(glm::vec3) * 2)
                .setStride(sizeof(DataTypes::Vertex))
                .build();

            auto elementBufferLayout = RenderSystem::VertexDataLayoutBuilder()
                .addAttribute("element", sizeof(unsigned int), RenderSystem::RS_FLOAT, RenderSystem::RS_FALSE, 0)
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

        void Mesh::destroy()
        {
            VAO = nullptr;
            VBO = nullptr;
            EBO = nullptr;

            std::vector<TextureUnitBinding>().swap(textures);
        }

        void Mesh::bind(RenderSystem::ShaderProgram *shader)
        {
            for (const auto &textureUnitBinding : textures) {
                shader->setTexture(textureUnitBinding.first, textureUnitBinding.second);
            }

            shader->bind();
            VAO->bind();
            EBO->bind(GL_ELEMENT_ARRAY_BUFFER);
        }

        void Mesh::render(size_t instanceCount)
        {
            //TODO: abstract the GL call here into a platform-independent mechanism of
            //some kind
            glDrawElementsInstanced(
                GL_TRIANGLES,
                elementCount,
                GL_UNSIGNED_INT,
                0, //offset into buffer containing elements (this buffer is elements only, so no offset necessary)
                instanceCount //size of MVP matrix buffer, handed down from SceneNode -> Model -> Mesh
            );
        }
    }
}
