#ifndef FLARE_GL_TEXTURE_H
#define FLARE_GL_TEXTURE_H

#include <memory>

#include <GL/gl3w.h>

namespace Flare
{
    namespace GL
    {
        class Texture
        {
            private:
                GLuint textureBuffer = 0;
                GLsizei numMipmapLevels = 0;
                GLenum internalFormat = GL_RGBA;
                GLsizei textureWidth = 0;
                GLsizei textureHeight = 0;
            public:
                Texture(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight);
        };
    }
}

#endif
