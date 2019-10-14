#include <flare/rendersystem/buffermanager.h>

namespace Flare
{
    namespace RenderSystem
    {
        Buffer *BufferManager::create(const std::string &name, const VertexDataLayout &bufferContentDescription)
        {
            return nullptr;
        }

        void BufferManager::destroy(const std::string &name)
        {
        }

        void BufferManager::resize(const std::string &name, size_t sizeInElements)
        {
        }
    }
}
