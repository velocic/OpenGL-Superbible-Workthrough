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
            destroyBuffers();
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

        void Mesh::populateBuffers()
        {
            auto vertexBufferLayout = RenderSystem::VertexDataLayoutBuilder()
                .addAttribute("position", sizeof(glm::vec3), GL_FLOAT, GL_FALSE, 0)
                .addAttribute("normal", sizeof(glm::vec3), GL_FLOAT, GL_FALSE, sizeof(glm::vec3))
                .addAttribute("uvCoords", sizeof(glm::vec2), GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2)
                .setStride(sizeof(DataTypes::Vertex))
                .build();

            // auto elementBufferLayout = RenderSystem::VertexDataLayoutBuilder()
            //     .addAttribute("element", sizeof(unsigned int), GL_FLOAT, GL_FALSE, 0)
            //     .setStride()

            //TODO: rendersystem-agnostic way of doing this?
            // VBO = std::make_unique<Flare::GL::Buffer>("vertexBuffer", vertexBufferLayout);
            VBO = RenderSystem::createBuffer("vertexBuffer", vertexBufferLayout);

        }

        void Mesh::destroyBuffers()
        {
        }

        void Mesh::render(RenderSystem::ShaderProgram *shader)
        {
        }
    }
}
