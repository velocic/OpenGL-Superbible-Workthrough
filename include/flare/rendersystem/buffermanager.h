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
                Buffer *createBuffer(const std::string &name, const VertexDataLayout &bufferContentDescription);
        };
    }
}

#endif
