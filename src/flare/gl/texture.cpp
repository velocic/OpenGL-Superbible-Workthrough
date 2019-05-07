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
        {
            glTextureStorage2D(textureBuffer, numMipmapLevels, internalFormat, textureWidth, textureHeight);
        }
    }
}
