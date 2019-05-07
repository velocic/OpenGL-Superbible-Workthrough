#ifndef FLARE_GL_TEXTURE_H
#define FLARE_GL_TEXTURE_H

namespace Flare
{
    namespace GL
    {
        class Texture
        {
            public:
                virtual void initialize() = 0;
                virtual void destroy() = 0;
                virtual void bind(GLenum target) = 0;
        };
    }
}

#endif
