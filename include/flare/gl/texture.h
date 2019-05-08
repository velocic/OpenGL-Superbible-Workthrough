#ifndef FLARE_GL_TEXTURE_H
#define FLARE_GL_TEXTURE_H

#include <GL/gl3w.h>

namespace Flare
{
    namespace GL
    {
        class Texture
        {
            public:
                virtual void initialize() = 0;
                virtual void destroy() = 0;
                virtual void bind() = 0;
        };
    }
}

#endif
