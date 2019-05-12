#include <flare/gl/texture.h>

namespace Flare
{
    namespace GL
    {
        Texture::Texture(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight)
        :
            numMipmapLevels(numMipmapLevels),
            internalFormat(internalFormat),
            textureWidth(textureWidth),
            textureHeight(textureHeight)
        {}

        Texture::~Texture()
        {}

        Texture::Texture(Texture&& other)
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

        Texture& Texture::operator=(Texture&& other)
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
    }
}
