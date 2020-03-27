#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace RenderSystem
    {
#ifdef RENDERSYSTEM_GL4
        std::unique_ptr<Buffer> createBuffer(const std::string &name, const VertexDataLayout& bufferContentDescription, BufferType bufferType)
        {
            return std::make_unique<Flare::GL::Buffer>(name, bufferContentDescription, bufferType);
        }

        std::unique_ptr<VertexArray> createVertexArray(const RenderSystem::ShaderProgram *shaderProgram, const std::vector<RenderSystem::VertexBufferVertexDataLayout> &requiredBufferLayouts)
        {
            return std::make_unique<Flare::GL::VertexArray>(shaderProgram, requiredBufferLayouts);
        }
#endif
    }
}
