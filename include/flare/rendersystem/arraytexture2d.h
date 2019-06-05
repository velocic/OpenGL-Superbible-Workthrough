#ifndef FLARE_RENDERSYSTEM_ARRAYTEXTURE2D_H
#define FLARE_RENDERSYSTEM_ARRAYTEXTURE2D_H

namespace Flare
{
    namespace RenderSystem
    {
        class ArrayTexture2D
        {
            public:
                virtual ~ArrayTexture2D() {}
                virtual void bufferPixelData(RSint level, RSint xOffset, RSint yOffset, RSint zOffset, RSsizei width, RSsizei height, RSenum format, RSenum type, const void *pixels, RSboolean generateMipmaps) = 0;
                virtual void clearPixelData(RSint level, RSint xOffset, RSint yOffset, RSint zOffset, RSsizei width, RSsizei height, RSenum format, RSenum type, const void *data) = 0;
        };
    }
}

#endif
