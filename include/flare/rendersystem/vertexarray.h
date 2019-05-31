#ifndef FLARE_RENDERSYSTEM_VERTEXARRAY_H
#define FLARE_RENDERSYSTEM_VERTEXARRAY_H

namespace Flare
{
    namespace RenderSystem
    {
        class VertexArray
        {
            public:
                virtual void bind() = 0;
                virtual void destroy() = 0;
                virtual void unbind() = 0;
        };
    }
}

#endif
