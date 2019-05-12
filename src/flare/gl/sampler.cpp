#include <flare/gl/sampler.h>

#include <stdexcept>

namespace Flare
{
    namespace GL
    {
        Sampler::Sampler(std::string_view name)
        :
            name(name)
        {
            initialize();
        }

        Sampler::~Sampler()
        {
            destroy();
        }

        Sampler::Sampler(Sampler&& other)
        :
            glSampler(other.glSampler)
        {
            other.glSampler = 0;
        }

        Sampler& Sampler::operator=(Sampler&& other)
        {
            glSampler = other.glSampler;

            other.glSampler = 0;

            return *this;
        }

        std::string_view Sampler::getName()
        {
            return name;
        }

        void Sampler::samplerParameteri(GLenum pName, GLint param)
        {
            glSamplerParameteri(glSampler, pName, param);
        }

        void Sampler::samplerParameterf(GLenum pName, GLfloat param)
        {
            glSamplerParameterf(glSampler, pName, param);
        }

        void Sampler::initialize()
        {
            if (glSampler != 0) {
                throw std::runtime_error("Attempting to initialize an OpenGL sampler twice.");
            }

            glCreateSamplers(1, &glSampler);
        }

        void Sampler::destroy()
        {
            glDeleteSamplers(1, &glSampler);
        }

        void Sampler::bind(GLuint unit)
        {
            if (glSampler == 0) {
                throw std::runtime_error("Attempting to bind an OpenGL sampler that has not been initialized.");
            }

            glBindSampler(unit, glSampler);
        }
    }
}
