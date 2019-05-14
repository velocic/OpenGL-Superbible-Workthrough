#include <flare/gl/arraytexture2d.h>

#include <algorithm>
#include <stdexcept>

namespace Flare
{
    namespace GL
    {
        ArrayTexture2D::ArrayTexture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight, GLsizei textureDepth)
        :
            Texture(numMipmapLevels, internalFormat, textureWidth),
            textureHeight(textureHeight),
            textureDepth(textureDepth)
        {
            initialize();
        }

        ArrayTexture2D::~ArrayTexture2D()
        {
            destroy();
        }

        ArrayTexture2D::ArrayTexture2D(ArrayTexture2D&& other)
        :
            Texture(std::move(other)),
            textureHeight(other.textureHeight),
            textureDepth(other.textureDepth)
        {
            other.textureHeight = 0;
            other.textureDepth = 0;
        }

        ArrayTexture2D& ArrayTexture2D::operator=(ArrayTexture2D&& other)
        {
            textureHeight = other.textureHeight;
            textureDepth = other.textureDepth;
            Texture::operator=(std::move(other));

            other.textureHeight = 0;
            other.textureDepth = 0;

            return *this;
        }

        void ArrayTexture2D::textureSubImage3D(GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, bool generateMipmaps)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to buffer data to an OpenGL array texture that is uninitialized.");
            }

            glTextureSubImage3D(
                glTexture,
                level,
                xOffset, yOffset,
                zOffset,
                width, height,
                1,
                format,
                type,
                pixels
            );

            //Mipmaps are generated from the image stored in level 0, so ignore requests to gen mipmaps if another level is being set
            if (generateMipmaps && level == 0) {
                glGenerateTextureMipmap(glTexture);
            }
        }

        void ArrayTexture2D::clearTexSubImage(GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to clear an OpenGL array texture that is uninitialized.");
            }

            glClearTexSubImage(
                glTexture,
                level,
                xOffset, yOffset,
                zOffset,
                width, height,
                1,
                format,
                type,
                data
            );
        }

        void ArrayTexture2D::initialize()
        {
            if (glTexture != 0) {
                throw std::runtime_error("Attempting to initialize an OpenGL texture that's already been initialized.");
            }

            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &glTexture);
            glTextureStorage3D(glTexture, numMipmapLevels, internalFormat, textureWidth, textureHeight, textureDepth);
        }

        void ArrayTexture2D::destroy()
        {
            glDeleteTextures(1, &glTexture);
        }

        void ArrayTexture2D::bind(GLuint textureUnitIndex)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to bind an OpenGL texture that's uninitialized.");
            }

            glBindTextureUnit(textureUnitIndex, glTexture);
        }
    }
}
