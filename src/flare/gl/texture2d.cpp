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
            internalFormat(internalFormat),
            textureWidth(textureWidth),
            textureHeight(textureHeight)
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
        }

        void Texture2D::textureSubImage2D(const GLvoid *pixels)
        {
            //TODO: determine if internalFormat represents integer or float data
            glTextureSubImage2D(
                glTexture,
                0,
                0, 0,
                textureWidth, textureHeight,
                internalFormat,
                //TODO: determine data type from internalFormat
                pixels
            );
        }

        void Texture2D::textureSubImage2D(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
        {
        }

        void Texture2D::initialize()
        {
            if (glTexture != 0) {
                //TODO: already initialized, handle that case
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &glTexture);
            glTextureStorage2D(glTexture, numMipmapLevels, internalFormat, textureWidth, textureHeight);
        }

        void Texture2D::destroy()
        {
            glDeleteTextures(1, &glTexture);
        }

        void Texture2D::bind(GLenum target)
        {
            if (glTexture == 0) {
                //TODO: we're currently uninitialized, handle that case
            }

            glBindTexture(target, glTexture);
        }
    }
}
