#ifndef FLARE_SCENEGRAPH_MESH_H
#define FLARE_SCENEGRAPH_MESH_H

#include <string>
#include <vector>

#include <glm-0.9.9/glm.hpp>

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
                RenderSystem::VertexArray VAO;
                RenderSystem::Buffer VBO;
                RenderSystem::Buffer EBO;

            public:
                Mesh(std::vector<DataTypes::Vertex> vertices, std::vector<unsigned int> indices, std::vector<RenderSystem::Texture *> textures);
                ~Mesh();
                Mesh(Mesh &&other);
                Mesh &operator=(Mesh &&other);
                Mesh(const Mesh &other) = delete;
                Mesh &operator=(const Mesh &other) = delete;
        };
    }
}

#endif
