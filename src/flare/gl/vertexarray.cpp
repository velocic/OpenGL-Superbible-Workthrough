#include <flare/gl/vertexarray.h>

namespace Flare
{
    namespace GL
    {
        VertexArray::VertexArray(const RenderSystem::ShaderProgram& shaderProgram,  const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers)
        :
            shaderProgram(shaderProgram),
            linkedBuffers(linkedBuffers)
        {
            glCreateVertexArrays(1, &VAO);
            bindAttributesToBuffers(VAO, shaderProgram, linkedBuffers);
        }

        VertexArray::VertexArray(VertexArray &&other)
        :
            shaderProgram(other.shaderProgram),
            linkedBuffers(other.linkedBuffers),
            VAO(other.VAO)
        {
            other.linkedBuffers.clear();
            other.VAO = 0;
        }

        VertexArray &VertexArray::operator=(VertexArray &&other)
        {
            shaderProgram = std::move(other.shaderProgram);
            linkedBuffers = std::move(other.linkedBuffers);
            VAO = other.VAO;

            other.linkedBuffers.clear();
            other.VAO = 0;

            return *this;
        }

        VertexArray::~VertexArray()
        {
            destroy();
        }

        void VertexArray::bind()
        {
            glBindVertexArray(VAO);
        }

        void VertexArray::bindAttributesToBuffers(GLuint VAO, const RenderSystem::ShaderProgram& shaderProgram, const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers)
        {
            for (size_t bufferBindingIndex = 0; bufferBindingIndex < linkedBuffers.size(); ++bufferBindingIndex) {
                const auto &buffer = linkedBuffers[bufferBindingIndex].get();
                const auto &bufferLayout = buffer.getContentDescription();
                glVertexArrayVertexBuffer(VAO, bufferBindingIndex, buffer.getId(), bufferLayout.offset, bufferLayout.stride);

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
                    glEnableVertexArrayAttrib(VAO, attributeIndex);
                    glEnableVertexAttribArray(bufferBindingIndex);
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
