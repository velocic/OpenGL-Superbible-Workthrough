#include <flare/scenegraph/mesh.h>

#include <flare/rendersystem/datalayout.h>
#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace SceneGraph
    {
        //TODO:move rather than copy?
        Mesh::Mesh(const std::vector<DataTypes::Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<RenderSystem::Texture *> &textures)
        :
            vertices(vertices),
            indices(indices),
            textures(textures)
        {
            populateBuffers();
        }

        Mesh::~Mesh()
        {
            destroy();
        }

        Mesh::Mesh(Mesh &&other)
        :
            vertices(std::move(other.vertices)),
            indices(std::move(other.indices)),
            textures(std::move(other.textures))
        {
        }

        Mesh &Mesh::operator=(Mesh &&other)
        {
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            textures = std::move(other.textures);

            return *this;
        }

        void Mesh::clearBuffers()
        {
            VBO = nullptr;
            EBO = nullptr;
        }

        void Mesh::populateBuffers()
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
            clearBuffers();

            std::vector<DataTypes::Vertex>().swap(vertices);
            std::vector<unsigned int>().swap(indices);
            std::vector<RenderSystem::Texture *>().swap(textures);
        }

        void Mesh::render(RenderSystem::ShaderProgram *shader)
        {
            //TODO: bind textures to the appropriate texture units
        }
    }
}
