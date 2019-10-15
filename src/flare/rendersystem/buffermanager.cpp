#include <flare/rendersystem/buffermanager.h>

#include <algorithm>
#include <flare/rendersystem/factory.h>

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

            return *this;
        }

        Buffer *BufferManager::create(const std::string &alias, const std::string &bufferName, const VertexDataLayout &bufferContentDescription)
        {
            auto newBuffer = RenderSystem::createBuffer(bufferName, bufferContentDescription);
            auto result = newBuffer.get();
            buffers.insert_or_assign(stringHasher(alias), std::move(newBuffer));
            return result;
        }

        void BufferManager::destroy(const std::string &alias)
        {
            buffers.erase(stringHasher(alias));
        }

        Buffer *BufferManager::resizeBytes(const std::string &alias, size_t sizeInBytes)
        {
            auto targetBufferIterator = buffers.find(stringHasher(alias));

            if (targetBufferIterator != buffers.end()) {
                auto &oldBuffer = targetBufferIterator->second;
                auto newBuffer = RenderSystem::createBuffer(oldBuffer->getAlias(), oldBuffer->getContentDescription());
                auto result = newBuffer.get();

                newBuffer->allocateBufferStorage(
                    sizeInBytes,
                    nullptr,
                    oldBuffer->getUsageFlags()
                );

                if (sizeInBytes > oldBuffer->getSizeInBytes()) {
                    newBuffer->copyRange(*oldBuffer, 0, 0, oldBuffer->getSizeInBytes());
                } else {
                    newBuffer->copyRange(*oldBuffer, 0, 0, sizeInBytes);
                }
                oldBuffer = std::move(newBuffer);

                return result;
            }

            throw std::runtime_error("Target buffer not found in resize operation");
            return nullptr;
        }

        Buffer *BufferManager::resizeElements(const std::string &alias, size_t sizeInElements)
        {
            auto targetBufferIterator = buffers.find(stringHasher(alias));

            if (targetBufferIterator != buffers.end()) {
                auto &oldBuffer = targetBufferIterator->second;
                auto newBuffer = RenderSystem::createBuffer(oldBuffer->getAlias(), oldBuffer->getContentDescription());
                auto result = newBuffer.get();
                auto sizeInBytes = sizeInElements * oldBuffer->getContentDescription().stride;

                newBuffer->allocateBufferStorage(
                    sizeInBytes,
                    nullptr,
                    oldBuffer->getUsageFlags()
                );
                if (sizeInBytes > oldBuffer->getSizeInBytes()) {
                    newBuffer->copyRange(*oldBuffer, 0, 0, oldBuffer->getSizeInBytes());
                } else {
                    newBuffer->copyRange(*oldBuffer, 0, 0, sizeInBytes);
                }
                oldBuffer = std::move(newBuffer);

                return result;
            }

            throw std::runtime_error("Target buffer not found in resize operation");
            return nullptr;
        }
    }
}
