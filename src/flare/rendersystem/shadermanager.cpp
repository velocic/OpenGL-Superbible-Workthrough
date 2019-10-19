#include <flare/rendersystem/shadermanager.h>

namespace Flare
{
    namespace RenderSystem
    {
        ShaderManager::~ShaderManager()
        {
            removeAll();
        }

        ShaderManager::ShaderManager(ShaderManager &&other)
        :
            shaders(std::move(other.shaders))
        {
        }

        ShaderManager &ShaderManager::operator=(ShaderManager &&other)
        {
            shaders = std::move(other.shaders);

            return *this;
        }

        RenderSystem::ShaderData ShaderManager::get(const std::string &alias) const
        {
            auto result = shaders.find(stringHasher(alias));

            if (result != shaders.end()) {
                return RenderSystem::ShaderData{
                    result->second.shader.get(),
                    result->second.vertexArray.get(),
                    result->second.hashedAlias
                };
            }

            return RenderSystem::ShaderData{};
        }

        void ShaderManager::insert(std::unique_ptr<Flare::RenderSystem::ShaderProgram> &&shader, std::unique_ptr<Flare::RenderSystem::VertexArray> &&vertexArray, const std::string &alias)
        {
            auto hashedAlias = stringHasher(alias);
            shaders.insert_or_assign(
                hashedAlias,
                ShaderData{
                    std::move(shader),
                    std::move(vertexArray),
                    hashedAlias
                }
            );
        }

        void ShaderManager::remove(const std::string &alias)
        {
            shaders.erase(stringHasher(alias));
        }

        void ShaderManager::removeAll()
        {
            shaders.clear();
        }
    }
}
