#ifndef FLARE_RENDERSYSTEM_BUFFERMANAGER_H
#define FLARE_RENDERSYSTEM_BUFFERMANAGER_H

#include <memory>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/datalayout.h>

namespace Flare
{
    namespace RenderSystem
    {
        class BufferManager
        {
            private:
            public:
                Buffer *create(const std::string &name, const VertexDataLayout &bufferContentDescription);
                void destroy(const std::string &name);
                void resize(const std::string &name, size_t sizeInElements);
        };
    }
}

#endif
