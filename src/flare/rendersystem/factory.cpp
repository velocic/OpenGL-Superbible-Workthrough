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
#endif
    }
}
