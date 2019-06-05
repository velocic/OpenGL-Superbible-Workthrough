#ifndef FLARE_SCENEGRAPH_MESH_H
#define FLARE_SCENEGRAPH_MESH_H

#include <memory>
#include <string>
#include <vector>

#include <glm-0.9.9/glm.hpp>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/vertexarray.h>
#include <flare/rendersystem/shaderprogram.h>
#include <flare/rendersystem/texture.h>
#include <flare/utility/datatypes.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Mesh
        {
            private:
                std::vector<DataTypes::Vertex> vertices;
                std::vector<unsigned int> indices;
                std::vector<RenderSystem::Texture *> textures;
                std::unique_ptr<RenderSystem::VertexArray> VAO;
                std::unique_ptr<RenderSystem::Buffer> VBO;
                std::unique_ptr<RenderSystem::Buffer> EBO;

                void populateBuffers();
            public:
                Mesh(const std::vector<DataTypes::Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<RenderSystem::Texture *> &textures);
                ~Mesh();
                Mesh(Mesh &&other);
                Mesh &operator=(Mesh &&other);
                Mesh(const Mesh &other) = delete;
                Mesh &operator=(const Mesh &other) = delete;

                void destroyBuffers();
                void render(RenderSystem::ShaderProgram *shader);
        };
    }
}

#endif
