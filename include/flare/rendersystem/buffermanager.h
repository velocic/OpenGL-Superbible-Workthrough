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

                Buffer *create(const std::string &name, const VertexDataLayout &bufferContentDescription);
                void destroy(const std::string &name);
                Buffer *resizeBytes(const std::string &name, size_t sizeInBytes);
                Buffer *resizeElements(const std::string &name, size_t sizeInElements);
        };
    }
}

#endif
