#ifndef FLARE_RENDERSYSTEM_TEXTURE_H
#define FLARE_RENDERSYSTEM_TEXTURE_H

#include <flare/rendersystem/datatypes.h>

namespace Flare
{
    namespace RenderSystem
    {
        class Texture
        {
            public:
                Texture(RSsizei numMipmapLevels, RSenum internalFormat, RSsizei textureWidth) {}
                virtual ~Texture() {};
                Texture(Texture&& other) {}
                Texture& operator=(Texture&& other) { return *this; }
                Texture(const Texture& other) = delete;
                Texture& operator=(const Texture& other) = delete;

                virtual void setParameter(RSenum name, RSfloat value) = 0;
                virtual void setParameter(RSenum name, RSint value) = 0;

                virtual void destroy() = 0;
                virtual void initialize() = 0;
                virtual void bind(RSuint textureUnitIndex) = 0;
                virtual void bindAsWritableFromShader(RSuint unit, RSint level, RSboolean layered, RSint layer, RSenum access, RSenum format) = 0;
        };
    }
}

#endif
