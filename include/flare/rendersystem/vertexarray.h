#ifndef FLARE_RENDERSYSTEM_VERTEXARRAY_H
#define FLARE_RENDERSYSTEM_VERTEXARRAY_H

#include <functional>

#include <flare/rendersystem/buffer.h>

namespace Flare
{
    namespace RenderSystem
    {
        class VertexArray
        {
            public:
                virtual ~VertexArray() {}

                virtual void bind() = 0;
                virtual void destroy() = 0;
                virtual void linkBuffers(const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers) = 0;
                virtual void unbind() = 0;
        };
    }
}

#endif
