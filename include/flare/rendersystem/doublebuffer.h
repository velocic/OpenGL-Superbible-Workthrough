#ifndef FLARE_RENDERSYSTEM_DOUBLEBUFFER_H
#define FLARE_RENDERSYSTEM_DOUBLEBUFFER_H

#include <memory>

#include <flare/rendersystem/buffer.h>

namespace Flare
{
    namespace RenderSystem
    {
        struct DoubleBuffer
        {
            std::unique_ptr<RenderSystem::Buffer> readBuffer;
            std::unique_ptr<RenderSystem::Buffer> writeBuffer;

            void destroy();
            void swap();
        };
    }
}

#endif
