#ifndef FLARE_SCENEGRAPH_MESH_H
#define FLARE_SCENEGRAPH_MESH_H

#include <memory>
#include <string>
#include <vector>
#include <variant>

#include <glm-0.9.9/glm.hpp>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/utility/datatypes.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Mesh
        {
            private:
                std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, nullptr_t> textures;
                std::unique_ptr<RenderSystem::Buffer> VBO;
                std::unique_ptr<RenderSystem::Buffer> EBO;
                size_t elementCount = 0;

                void populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
            public:
                Mesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PhongMaterialTextures &textures);
                Mesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PBRMaterialTextures &textures);
                Mesh(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
                ~Mesh();
                Mesh(Mesh &&other);
                Mesh &operator=(Mesh &&other);
                Mesh(const Mesh &other) = delete;
                Mesh &operator=(const Mesh &other) = delete;

                void destroy();
                void bind(RenderSystem::ShaderData shaderData);
                void render(size_t instanceCount);
        };
    }
}

#endif
