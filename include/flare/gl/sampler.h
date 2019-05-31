#ifndef FLARE_GL_SAMPLER_H
#define FLARE_GL_SAMPLER_H

#include <string>
#include <string_view>

#include <GL/gl3w.h>

#include <flare/rendersystem/sampler.h>

namespace Flare
{
    namespace GL
    {
        class Sampler : public RenderSystem::Sampler
        {
            private:
                GLuint glSampler = 0;
                std::string name;
            public:
                Sampler(std::string_view name);
                ~Sampler();
                Sampler(Sampler&& other);
                Sampler& operator=(Sampler&& other);
                Sampler(const Sampler& other) = delete;
                Sampler& operator=(const Sampler& other) = delete;

                virtual std::string_view getName() override;
                virtual void samplerParameteri(GLenum pname, GLint param) override;
                virtual void samplerParameterf(GLenum pname, GLfloat param) override;
                virtual void initialize() override;
                virtual void destroy() override;
                virtual void bind(GLuint unit) override;
        };
    }
}

#endif
