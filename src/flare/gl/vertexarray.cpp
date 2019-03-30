#include <flare/gl/vertexarray.h>

#include <iostream>

namespace Flare
{
    namespace GL
    {
        VertexArray::VertexArray(const ShaderProgram& shaderProgram, DataLayout dataLayout)
        :
            shaderProgram(shaderProgram),
            dataLayout(dataLayout)
        {
        }

        bool VertexArray::addAttribute(GLuint VAO, const std::string &attributeName)
        {
            if (shaderProgram.isShaderProgramValid() == false) {
                //TODO: Replace cout with logfile writing
                std::cout << "addAttribute called on Material instance with invalid state." << std::endl;
                return false;
            }

            if (VAO == 0) {
                std::cout << "addAttribute called on null vertex array object." << std::endl;
                return false;
            }

            glBindVertexArray(VAO);
            GLint attributeLocation = glGetAttribLocation(shaderProgram.getShaderProgramId(), attributeName.c_str());

            if (attributeLocation == -1) {
                return false;
            }

            shaderAttributes[VAO].emplace_back(attributeName, attributeLocation);
            glEnableVertexArrayAttrib(VAO, attributeLocation);
            glBindVertexArray(0);

            return true;
        }

        void VertexArray::setGLVertexAttribPointer(GLuint VAO, const std::string &attributeName, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *glPointer)
        {
            if (isShaderProgramValid() == false) {
                //TODO: Replace cout with logfile writing
                std::cout << "setGLVertexAttribPointer called on Material instance with invalid state." << std::endl;
                return;
            }

            if (VAO == 0) {
                std::cout << "setGLVertexAttribPointer called with a null vertex array object." << std::endl;
            }

            glBindVertexArray(VAO);

            glVertexAttribPointer(
                getAttribute(VAO, attributeName),
                size,
                type,
                normalized,
                stride,
                glPointer
            );

            glBindVertexArray(0);
        }
    }
}
