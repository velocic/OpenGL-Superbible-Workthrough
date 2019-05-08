#include <flare/gl/texture2d.h>

namespace Flare
{
    namespace GL
    {
        Texture2D::Texture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight)
        :
            numMipmapLevels(numMipmapLevels),
            internalFormat(internalFormat),
            textureWidth(textureWidth),
            textureHeight(textureHeight)
        {
            initialize();
        }

        Texture2D::~Texture2D()
        {
            destroy();
        }

        Texture2D::Texture2D(Texture2D&& other)
        :
            numMipmapLevels(other.numMipmapLevels),
            internalFormat(other.internalFormat),
            textureWidth(other.textureWidth),
            textureHeight(other.textureHeight)
        {
            other.glTexture = 0;
            other.numMipmapLevels = 0;
            other.internalFormat = GL_RGBA;
            other.textureWidth = 0;
            other.textureHeight = 0;
        }

        Texture2D& Texture2D::operator=(Texture2D&& other)
        {
            glTexture = other.glTexture;
            numMipmapLevels = other.numMipmapLevels;
            textureWidth = other.textureWidth;
            textureHeight = other.textureHeight;

            other.glTexture = 0;
            other.numMipmapLevels = 0;
            other.internalFormat = GL_RGBA;
            other.textureWidth = 0;
            other.textureHeight = 0;

            return *this;
        }

        void Texture2D::textureSubImage2D(const GLvoid *pixels, GLenum type)
        {
            glTextureSubImage2D(
                glTexture,
                0,
                0, 0,
                textureWidth, textureHeight,
                internalFormat,
                type,
                pixels
            );
        }

        void Texture2D::textureSubImage2D(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
        {
            glTextureSubImage2D(
                glTexture,
                level,
                xOffset, yOffset,
                width, height,
                format,
                type,
                pixels
            );
        }

        void Texture2D::clearTexSubImage(GLenum type, const void *data)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to clear an OpenGL texture that is uninitialized.");
            }

            glClearTexSubImage(
                glTexture,
                0,
                0, 0, 0,
                textureWidth, textureHeight, 0,
                internalFormat,
                type,
                data
            );
        }

        void Texture2D::clearTexSubImage(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to clear an OpenGL texture that is uninitialized.");
            }

            glClearTexSubImage(
                glTexture,
                level,
                xOffset, yOffset, 0,
                width, height, 0,
                format,
                type,
                data
            );
        }

        void Texture2D::setTextureParameters(GLuint wrapModeS, GLuint wrapModeT, GLuint minFilter, GLuint magFilter, bool generateMipmaps)
        {
            bind();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

            if (generateMipmaps) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void Texture2D::initialize()
        {
            if (glTexture != 0) {
                throw std::runtime_error("Attempting to initialize an OpenGL texture that's already initialized.");
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &glTexture);
            glTextureStorage2D(glTexture, numMipmapLevels, internalFormat, textureWidth, textureHeight);
        }

        void Texture2D::destroy()
        {
            glDeleteTextures(1, &glTexture);
        }

        void Texture2D::bind()
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to bind an OpenGL texture that's uninitialized");
            }

            glBindTexture(GL_TEXTURE_2D, glTexture);
        }
    }
}
