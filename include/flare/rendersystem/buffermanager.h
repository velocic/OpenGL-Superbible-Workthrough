#ifndef FLARE_RENDERSYSTEM_BUFFERMANAGER_H
#define FLARE_RENDERSYSTEM_BUFFERMANAGER_H

#include <memory>
#include <unordered_map>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/datalayout.h>

namespace Flare
{
    namespace RenderSystem
    {
        class BufferManager
        {
            private:
                std::unordered_map<size_t, std::unique_ptr<Buffer>> buffers;
                std::hash<std::string> stringHasher;
            public:
                BufferManager() {}
                ~BufferManager() {}
                BufferManager(BufferManager &&other);
                BufferManager &operator=(BufferManager &&other);
                BufferManager(const BufferManager &other) = delete;
                BufferManager &operator=(const BufferManager &other) = delete;

                Buffer *create(const std::string &alias, const std::string &bufferName, const VertexDataLayout &bufferContentDescription);
                void destroy(const std::string &alias);
                Buffer *resizeBytes(const std::string &alias, size_t sizeInBytes);
                Buffer *resizeElements(const std::string &alias, size_t sizeInElements);
        };
    }
}

#endif
