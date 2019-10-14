#include <flare/rendersystem/buffermanager.h>

namespace Flare
{
    namespace RenderSystem
    {
        BufferManager::BufferManager(BufferManager &&other)
        :
            buffers(std::move(other.buffers))
        {
        }

        BufferManager &BufferManager::operator=(BufferManager &&other)
        {
            buffers = std::move(other.buffers);
        }

        Buffer *BufferManager::create(const std::string &name, const VertexDataLayout &bufferContentDescription)
        {
            return nullptr;
        }

        void BufferManager::destroy(const std::string &name)
        {
        }

        Buffer *BufferManager::resizeBytes(const std::string &name, size_t sizeInBytes)
        {
            return nullptr;
        }

        Buffer *BufferManager::resizeElements(const std::string &name, size_t sizeInElements)
        {
            return nullptr;
        }
    }
}
