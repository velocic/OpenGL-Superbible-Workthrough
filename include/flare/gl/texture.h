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
                GLsizei numMipmapLevels = 1;
                GLenum internalFormat = GL_RGBA;
                GLsizei textureWidth = 0;
            public:
                Texture(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth);
                virtual ~Texture();
                Texture(Texture&& other);
                Texture& operator=(Texture&& other);
                Texture(const Texture& other) = delete;
                Texture& operator=(const Texture& other) = delete;

                virtual void textureParameterf(GLenum pname, GLfloat param);
                virtual void textureParameteri(GLenum pname, GLint param);

                virtual void destroy() = 0;
                virtual void initialize() = 0;
                virtual void bind(GLuint textureUnitIndex) = 0;
                virtual void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) = 0;
        };
    }
}

#endif
