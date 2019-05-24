#ifndef FLARE_RENDERSYSTEM_TEXTURE2D_H
#define FLARE_RENDERSYSTEM_TEXTURE2D_H

#include <flare/rendersystem/datatypes.h>

namespace Flare
{
    namespace RenderSystem
    {
        class Texture2D
        {
            public:
                Texture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight) {}
                virtual ~Texture2D() {}
                Texture2D(Texture2D &&other) {}
                Texture2D& operator=(Texture2D &&other) { return *this; }
                Texture2D(const Texture2D &other) = delete;
                Texture2D& operator=(const Texture2D &other) = delete;

                virtual void bufferPixelData(RSenum format, RSenum type, const void *pixels, RSboolean generateMipmaps) = 0;
                virtual void bufferPixelData(RSint level, RSint xOffset, RSint yOffset, RSsizei width, RSsizei height, RSenum format, RSenum type, const void *pixels, RSboolean generateMipmaps) = 0;
                virtual void clearPixelData(RSenum format, RSenum type, const void *data) = 0;
                virtual void clearPixelData(RSint level, RSint xOffset, RSint yOffset, RSsizei width, RSsizei height, RSenum format, RSenum type, const void *data) = 0;
        };
    }
}

#endif
