#ifndef FLARE_GL_VERTEXARRAY_H
#define FLARE_GL_VERTEXARRAY_H

#include <string>

#include <GL/gl3w.h>
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
                const DataLayout dataLayout;

                //Calls glVertexAttribPointer, but allows use of shader attribute location
                //by name. Must be called after proper VBO is bound, which needs to happen
                //externally from this class.
                //Defaults values assume non-normalized, tightly-packed data buffer
                void setGLVertexAttribPointer(
                    GLuint VAO,
                    const std::string &attributeName,
                    GLint size,
                    GLenum type,
                    GLboolean normalized = GL_FALSE,
                    GLsizei stride = 0,
                    const GLvoid *glPointer = 0
                );
                bool addAttribute(GLuint VAO, const std::string &attributeName);
            public:
                VertexArray(const ShaderProgram& shaderProgram, DataLayout dataLayout);

                GLint getAttribute(GLuint VAO, const std::string &attributeName);
        };
    }
}

#endif
