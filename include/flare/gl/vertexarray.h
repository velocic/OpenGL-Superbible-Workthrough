#ifndef FLARE_GL_VERTEXARRAY_H
#define FLARE_GL_VERTEXARRAY_H

#include <functional>
#include <string>

#include <GL/gl3w.h>
#include <flare/gl/buffer.h>
#include <flare/gl/shaderprogram.h>

namespace Flare
{
    namespace GL
    {
        class VertexArray
        {
            private:
                std::reference_wrapper<const ShaderProgram> shaderProgram;
                std::vector<std::reference_wrapper<const Buffer>> linkedBuffers;
                GLuint VAO = 0;

                void bindAttributesToBuffers(GLuint VAO, const ShaderProgram& shaderProgram, const std::vector<std::reference_wrapper<const Buffer>> &linkedBuffers);
                //TODO: connecting this VAO to different buffers with the same layout is easy for OpenGL. All that needs to be done is
                //to call glVertexArrayAttribBinding for each attribute, but referencing the new buffers instead. Should add a function
                //for this, but it requires the idea of a const set of buffers permanently linked to this instance to be expanded to be
                //more flexible
            public:
                VertexArray(const ShaderProgram &shaderProgram, const std::vector<std::reference_wrapper<const Buffer>> &linkedBuffers);
                VertexArray(VertexArray &&other);
                VertexArray &operator=(VertexArray &&other);
                VertexArray(const VertexArray &other) = delete;
                VertexArray &operator=(const VertexArray &other) = delete;
                ~VertexArray();

                void bind();
                void destroy();
                void unbind();
        };
    }
}

#endif
