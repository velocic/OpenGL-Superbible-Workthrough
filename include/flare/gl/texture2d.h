#ifndef FLARE_GL_TEXTURE2D_H
#define FLARE_GL_TEXTURE2D_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/gl/texture.h>

namespace Flare
{
    namespace GL
    {
        class Texture2D : public Texture
        {
            private:
            public:
                Texture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight);
                virtual ~Texture2D() override;
                Texture2D(Texture2D&& other);
                Texture2D& operator=(Texture2D&& other);
                Texture2D(const Texture2D& other) = delete;
                Texture2D& operator=(const Texture2D& other) = delete;

                void textureSubImage2D(const GLvoid *pixels, GLenum type, bool generateMipmaps);
                void textureSubImage2D(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, bool generateMipmaps);
                void clearTexSubImage(GLenum type, const void *data);
                void clearTexSubImage(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
                void initialize() override;
                void destroy() override;
                void bind(GLuint textureUnitIndex) override;
        };
    }
}

#endif
