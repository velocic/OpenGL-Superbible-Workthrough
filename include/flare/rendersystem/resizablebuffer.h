#ifndef FLARE_RENDERSYSTEM_RESIZABLEBUFFER_H
#define FLARE_RENDERSYSTEM_RESIZABLEBUFFER_H

#include <memory>
#include <unordered_map>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/datalayout.h>

namespace Flare
{
    namespace RenderSystem
    {
        class ResizableBuffer
        {
            private:
                std::unique_ptr<Buffer> buffer;
            public:
                ResizableBuffer() {}
                ~ResizableBuffer() {}
                ResizableBuffer(ResizableBuffer &&other);
                ResizableBuffer &operator=(ResizableBuffer &&other);
                ResizableBuffer(const ResizableBuffer &other) = delete;
                ResizableBuffer &operator=(const ResizableBuffer &other) = delete;

                Buffer *create(const std::string &bufferName, const VertexDataLayout &bufferContentDescription);
                void destroy();
                Buffer *resizeBytes(size_t sizeInBytes);
                Buffer *resizeElements(size_t sizeInElements);
        };
    }
}

#endif
