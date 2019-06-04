#ifndef FLARE_RENDERSYSTEM_SAMPLER_H
#define FLARE_RENDERSYSTEM_SAMPLER_H

#include <flare/rendersystem/datatypes.h>

namespace Flare
{
    namespace RenderSystem
    {
        class Sampler
        {
            public:
                virtual size_t getName() = 0;
                virtual void samplerParameteri(RSenum pname, RSint param) = 0;
                virtual void samplerParameterf(RSenum pname, RSfloat param) = 0;
                virtual void initialize() = 0;
                virtual void destroy() = 0;
                virtual void bind(RSuint unit) = 0;
        };
    }
}

#endif
