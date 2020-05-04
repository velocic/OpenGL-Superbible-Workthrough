#ifndef FLARE_RENDERSYSTEM_TEXTUREBUFFER_H
#define FLARE_RENDERSYSTEM_TEXTUREBUFFER_H

#include <flare/rendersystem/datatypes.h>
#include <flare/rendersystem/buffer.h>

namespace Flare
{
    namespace RenderSystem
    {
        class TextureBuffer
        {
            public:
                virtual ~TextureBuffer() {}

                virtual void attachBufferStorage(const RenderSystem::Buffer &buffer) = 0;
        };
    }
}

#endif
