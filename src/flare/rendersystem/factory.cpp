#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace RenderSystem
    {
#ifdef RENDERSYSTEM_GL4
        std::unique_ptr<Buffer> createBuffer(const std::string &name, const VertexDataLayout& bufferContentDescription)
        {
            return std::make_unique<Flare::GL::Buffer>(name, bufferContentDescription);
        }

        std::unique_ptr<VertexArray> createVertexArray(const RenderSystem::ShaderProgram *shaderProgram, const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers)
        {
            return std::make_unique<Flare::GL::VertexArray>(shaderProgram, linkedBuffers);
        }
#endif
    }
}
