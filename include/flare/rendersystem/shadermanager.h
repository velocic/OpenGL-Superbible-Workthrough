#ifndef FLARE_RENDERSYSTEM_SHADERMANAGER_H
#define FLARE_RENDERSYSTEM_SHADERMANAGER_H

#include <memory>
#include <unordered_map>

#include <flare/rendersystem/vertexarray.h>
#include <flare/rendersystem/shaderprogram.h>

namespace Flare
{
    namespace RenderSystem
    {
        struct ShaderData
        {
            Flare::RenderSystem::ShaderProgram *shader = nullptr;
            Flare::RenderSystem::VertexArray *vertexArray = nullptr;
            size_t hashedAlias = 0;
        };

        class ShaderManager
        {
            private:
                struct ShaderData
                {
                    std::unique_ptr<Flare::RenderSystem::ShaderProgram> shader = nullptr;
                    std::unique_ptr<Flare::RenderSystem::VertexArray> vertexArray = nullptr;
                    size_t hashedAlias = 0;
                };

                std::unordered_map<size_t, ShaderData> shaders;
                std::hash<std::string> stringHasher;
            public:
                ShaderManager() = default;
                ~ShaderManager();
                ShaderManager(ShaderManager &&other);
                ShaderManager &operator=(ShaderManager &&other);
                ShaderManager(const ShaderManager &other) = delete;
                ShaderManager &operator=(const ShaderManager &other) = delete;

                RenderSystem::ShaderData get(const std::string &alias) const;
                void insert(std::unique_ptr<Flare::RenderSystem::ShaderProgram> &&shader, std::unique_ptr<Flare::RenderSystem::VertexArray> &&vertexArray, const std::string &alias);
                void remove(const std::string &alias);
                void removeAll();
        };
    }
}

#endif
