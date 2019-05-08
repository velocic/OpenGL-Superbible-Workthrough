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
                GLuint glTexture = 0;
                GLsizei numMipmapLevels = 0;
                GLenum internalFormat = GL_RGBA;
                GLsizei textureWidth = 0;
                GLsizei textureHeight = 0;
            public:
                Texture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight);
                ~Texture2D();
                Texture2D(Texture2D&& other);
                Texture2D& operator=(Texture2D&& other);
                Texture2D(const Texture2D& other) = delete;
                Texture2D& operator=(const Texture2D& other) = delete;

                void textureSubImage2D(const GLvoid *pixels, GLenum type);
                void textureSubImage2D(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
                void clearTexSubImage(GLenum type, const void *data);
                void clearTexSubImage(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
                void setTextureParameters(GLuint wrapModeS, GLuint wrapModeT, GLuint minFilter, GLuint magFilter, bool generateMipmaps);
                void initialize() override;
                void destroy() override;
                void bind() override;
        };
    }
}

#endif
