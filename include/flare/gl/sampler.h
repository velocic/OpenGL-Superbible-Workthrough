#ifndef FLARE_GL_SAMPLER_H
#define FLARE_GL_SAMPLER_H

#include <GL/gl3w.h>

namespace Flare
{
    namespace GL
    {
        class Sampler
        {
            private:
                GLuint glSampler = 0;
            public:
                Sampler();
                ~Sampler();
                Sampler(Sampler&& other);
                Sampler& operator=(Sampler&& other);
                Sampler(const Sampler& other) = delete;
                Sampler& operator=(const Sampler& other) = delete;

                void samplerParameteri(GLenum pname, GLint param);
                void samplerParameterf(GLenum pname, GLfloat param);
                void initialize();
                void destroy();
                void bind(GLuint unit);
        };
    }
}

#endif
