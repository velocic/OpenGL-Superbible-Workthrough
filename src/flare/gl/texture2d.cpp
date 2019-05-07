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

        void Texture2D::initialize()
        {
            if (textureBuffer != 0) {
                //TODO: already initialized, handle that case
            }

            glTextureStorage2D(textureBuffer, numMipmapLevels, internalFormat, textureWidth, textureHeight);
        }

        void Texture2D::destroy()
        {
        }

        void Texture2D::bind(GLenum target)
        {
            if (textureBuffer == 0) {
                //TODO: we're currently uninitialized, handle that case
            }

            glBindTexture(target, textureBuffer);
        }
    }
}
