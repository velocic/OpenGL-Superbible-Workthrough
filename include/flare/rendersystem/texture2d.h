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
                virtual ~Texture2D() {}

                virtual void bufferPixelData(RSenum format, RSenum type, const void *pixels, RSboolean generateMipmaps) = 0;
                virtual void bufferPixelData(RSint level, RSint xOffset, RSint yOffset, RSsizei width, RSsizei height, RSenum format, RSenum type, const void *pixels, RSboolean generateMipmaps) = 0;
                virtual void clearPixelData(RSenum format, RSenum type, const void *data) = 0;
                virtual void clearPixelData(RSint level, RSint xOffset, RSint yOffset, RSsizei width, RSsizei height, RSenum format, RSenum type, const void *data) = 0;
        };
    }
}

#endif
