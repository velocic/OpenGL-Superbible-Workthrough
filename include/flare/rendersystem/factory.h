#ifndef FLARE_RENDERSYSTEM_FACTORY_H
#define FLARE_RENDERSYSTEM_FACTORY_H

#ifdef RENDERSYSTEM_GL4
#include <flare/gl/buffer.h>
#endif

namespace Flare
{
    namespace RenderSystem
    {
        std::unique_ptr<Buffer> createBuffer(const std::string &name, const VertexDataLayout& bufferContentDescription);
    }
}

#endif
