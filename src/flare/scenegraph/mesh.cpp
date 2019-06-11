#include <flare/scenegraph/mesh.h>

#include <flare/rendersystem/datalayout.h>
#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace SceneGraph
    {
        Mesh::Mesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::vector<TextureUnitBinding> &&textures)
        :
            textures(textures)
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
            textures(std::move(other.textures))
        {
        }

        Mesh &Mesh::operator=(Mesh &&other)
        {
            textures = std::move(other.textures);

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
            shader->bind();
            VAO->bind();

            for (const auto &textureUnitBinding : textures) {
                shader->setTexture(textureUnitBinding.first, textureUnitBinding.second);
            }
        }
    }
}
