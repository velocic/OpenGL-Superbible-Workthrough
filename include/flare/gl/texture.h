#ifndef FLARE_GL_TEXTURE_H
#define FLARE_GL_TEXTURE_H

#include <GL/gl3w.h>

namespace Flare
{
    namespace GL
    {
        class Texture
        {
            protected:
                GLuint glTexture = 0;
                GLsizei numMipmapLevels = 0;
                GLenum internalFormat = GL_RGBA;
                GLsizei textureWidth = 0;
                GLsizei textureHeight = 0;
            public:
                Texture(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight);
                virtual ~Texture();
                Texture(Texture&& other);
                Texture& operator=(Texture&& other);
                Texture(const Texture& other) = delete;
                Texture& operator=(const Texture& other) = delete;

                virtual void destroy() = 0;
                virtual void initialize() = 0;
                virtual void bind(GLuint textureUnitIndex) = 0;
        };
    }
}

#endif
