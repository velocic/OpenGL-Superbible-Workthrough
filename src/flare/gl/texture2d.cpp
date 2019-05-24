#include <flare/gl/texture2d.h>

#include <algorithm>
#include <stdexcept>

namespace Flare
{
    namespace GL
    {
        Texture2D::Texture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight)
        :
            Texture(numMipmapLevels, internalFormat, textureWidth),
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
            Texture(std::move(other)),
            textureHeight(other.textureHeight)
        {
            other.textureHeight = 0;
        }

        Texture2D& Texture2D::operator=(Texture2D&& other)
        {
            textureHeight = other.textureHeight;
            Texture::operator=(std::move(other));

            other.textureHeight = 0;

            return *this;
        }

        void Texture2D::textureSubImage2D(GLenum format, GLenum type, const GLvoid *pixels, bool generateMipmaps)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to buffer data to an OpenGL texture that is uninitialized.");
            }

            glTextureSubImage2D(
                glTexture,
                0,
                0, 0,
                textureWidth, textureHeight,
                format,
                type,
                pixels
            );

            if (generateMipmaps) {
                glGenerateTextureMipmap(glTexture);
            }
        }

        void Texture2D::textureSubImage2D(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, bool generateMipmaps)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to buffer data to an OpenGL texture that is uninitialized.");
            }

            glTextureSubImage2D(
                glTexture,
                level,
                xOffset, yOffset,
                width, height,
                format,
                type,
                pixels
            );

            //Mipmaps are generated from the image stored in level 0, so ignore requests to gen mipmaps if another level is being set
            if (generateMipmaps && level == 0) {
                glGenerateTextureMipmap(glTexture);
            }
        }

        void Texture2D::clearTexSubImage(GLenum format, GLenum type, const void *data)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to clear an OpenGL texture that is uninitialized.");
            }

            glClearTexSubImage(
                glTexture,
                0,
                0, 0, 0,
                textureWidth, textureHeight, 0,
                format,
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

        void Texture2D::bind(GLuint textureUnitIndex)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to bind an OpenGL texture that's uninitialized");
            }

            glBindTextureUnit(textureUnitIndex, glTexture);
        }

        void Texture2D::bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to bind an OpenGL image texture that's uninitialized");
            }

            glBindImageTexture(
                unit,
                glTexture,
                level,
                false,
                0,
                access,
                format
            );
        }

        void Texture2D::bindAsWritableFromShader(RenderSystem::RSuint unit, RenderSystem::RSint level, RenderSystem::RSboolean layered, RenderSystem::RSint layer, RenderSystem::RSenum access, RenderSystem::RSenum format)
        {
            bindImageTexture(unit, level, layered, layer, access, format);
        }
    }
}
