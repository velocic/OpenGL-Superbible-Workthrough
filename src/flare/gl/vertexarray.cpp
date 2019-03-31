#include <flare/gl/vertexarray.h>

#include <iostream>

namespace Flare
{
    namespace GL
    {
        VertexArray::VertexArray(const ShaderProgram& shaderProgram,  const std::vector<std::reference_wrapper<const Buffer>> &linkedBuffers)
        :
            shaderProgram(shaderProgram),
            linkedBuffers(linkedBuffers)
        {
            glCreateVertexArrays(1, &VAO);
            bindAttributesToBuffers(VAO, shaderProgram, linkedBuffers);
        }

        VertexArray::~VertexArray()
        {
            destroy();
        }

        void VertexArray::bind()
        {
            glBindVertexArray(VAO);
        }

        void VertexArray::bindAttributesToBuffers(GLuint VAO, const ShaderProgram& shaderProgram, const std::vector<std::reference_wrapper<const Buffer>> &linkedBuffers)
        {
            for (size_t bufferBindingIndex = 0; bufferBindingIndex < linkedBuffers.size(); ++bufferBindingIndex) {
                const auto &buffer = linkedBuffers[bufferBindingIndex].get();
                const auto &bufferLayout = buffer.getBufferContentDescription();
                glVertexArrayVertexBuffer(VAO, bufferBindingIndex, buffer.getName(), bufferLayout.offset, bufferLayout.stride);

                for (const auto &vertexAttribute : bufferLayout.vertexAttributes) {
                    auto attributeIndex = shaderProgram.getAttribute(vertexAttribute.name);

                    glVertexArrayAttribBinding(VAO, attributeIndex, bufferBindingIndex);
                    glVertexArrayAttribFormat(
                        VAO,
                        attributeIndex,
                        vertexAttribute.size,
                        vertexAttribute.type,
                        vertexAttribute.normalized,
                        vertexAttribute.relativeOffset
                    );
                    glEnableVertexAttribArray(attributeIndex);
                }
            }
        }

        void VertexArray::destroy()
        {
            glDeleteVertexArrays(1, &VAO);
            linkedBuffers.clear();
        }

        void VertexArray::unbind()
        {
            glBindVertexArray(0);
        }
    }
}
