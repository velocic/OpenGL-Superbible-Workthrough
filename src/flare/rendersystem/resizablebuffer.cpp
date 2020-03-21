#include <flare/rendersystem/resizablebuffer.h>

#include <algorithm>
#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace RenderSystem
    {
        ResizableBuffer::ResizableBuffer(ResizableBuffer &&other)
        :
            buffer(std::move(other.buffer))
        {
        }

        ResizableBuffer &ResizableBuffer::operator=(ResizableBuffer &&other)
        {
            buffer = std::move(other.buffer);

            return *this;
        }

        Buffer *ResizableBuffer::create(const std::string &bufferName, const VertexDataLayout &bufferContentDescription)
        {
            buffer = RenderSystem::createBuffer(bufferName, bufferContentDescription);
            return buffer.get();
        }

        void ResizableBuffer::destroy()
        {
            buffer = nullptr;
        }

        Buffer *ResizableBuffer::resizeBytes(size_t sizeInBytes)
        {
            auto newBuffer = RenderSystem::createBuffer(buffer->getAlias(), buffer->getContentDescription());

            newBuffer->allocateBufferStorage(
                sizeInBytes,
                nullptr,
                buffer->getUsageFlags()
            );

            if (sizeInBytes > buffer->getSizeInBytes()) {
                newBuffer->copyRange(*buffer, 0, 0, buffer->getSizeInBytes());
            } else {
                newBuffer->copyRange(*buffer, 0, 0, sizeInBytes);
            }
            buffer = std::move(newBuffer);

            return buffer.get();
        }

        Buffer *ResizableBuffer::resizeElements(size_t sizeInElements)
        {
            auto newBuffer = RenderSystem::createBuffer(buffer->getAlias(), buffer->getContentDescription());
            auto sizeInBytes = sizeInElements * buffer->getContentDescription().getStride();

            newBuffer->allocateBufferStorage(
                sizeInBytes,
                nullptr,
                buffer->getUsageFlags()
            );
            if (sizeInBytes > buffer->getSizeInBytes()) {
                newBuffer->copyRange(*buffer, 0, 0, buffer->getSizeInBytes());
            } else {
                newBuffer->copyRange(*buffer, 0, 0, sizeInBytes);
            }
            buffer = std::move(newBuffer);

            return buffer.get();
        }
    }
}
