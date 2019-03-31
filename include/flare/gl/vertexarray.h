#ifndef FLARE_GL_VERTEXARRAY_H
#define FLARE_GL_VERTEXARRAY_H

#include <functional>
#include <string>

#include <GL/gl3w.h>
#include <flare/gl/buffer.h>
#include <flare/gl/shaderprogram.h>
#include <flare/gl/datalayout.h>

namespace Flare
{
    namespace GL
    {
        class VertexArray
        {
            private:
                const ShaderProgram& shaderProgram;
                // std::vector<const Buffer &> linkedBuffers;
                std::vector<std::reference_wrapper<const Buffer>> linkedBuffers;
                GLuint VAO = 0;

                void bindAttributesToBuffers(GLuint VAO, const ShaderProgram& shaderProgram, const std::vector<std::reference_wrapper<const Buffer>> &linkedBuffers);
            public:
                VertexArray(const ShaderProgram& shaderProgram, const std::vector<std::reference_wrapper<const Buffer>> &linkedBuffers);
                ~VertexArray();

                void bind();
                void destroy();
                void unbind();
        };
    }
}

#endif
