#include <flare/gl/vertexarray.h>

namespace Flare
{
    namespace GL
    {
        VertexArray::VertexArray(const RenderSystem::ShaderProgram *shaderProgram,  const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers)
        :
            shaderProgram(shaderProgram)
        {
            glCreateVertexArrays(1, &VAO);
            configureAttributesFromInitialBuffers(VAO, *shaderProgram, linkedBuffers);
        }

        VertexArray::VertexArray(VertexArray &&other)
        :
            shaderProgram(other.shaderProgram),
            linkedBufferBindingIndices(std::move(other.linkedBufferBindingIndices)),
            VAO(other.VAO)
        {
            other.shaderProgram = nullptr;
            other.VAO = 0;
        }

        VertexArray &VertexArray::operator=(VertexArray &&other)
        {
            shaderProgram = other.shaderProgram;
            linkedBufferBindingIndices = std::move(other.linkedBufferBindingIndices);
            VAO = other.VAO;

            other.shaderProgram = nullptr;
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

        void VertexArray::configureAttributesFromInitialBuffers(GLuint VAO, const RenderSystem::ShaderProgram& shaderProgram, const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers)
        {
            for (size_t bufferBindingIndex = 0; bufferBindingIndex < linkedBuffers.size(); ++bufferBindingIndex) {
                const auto &buffer = linkedBuffers[bufferBindingIndex].get();
                const auto &bufferLayout = buffer.getContentDescription();

                linkedBufferBindingIndices[buffer.getName()] = bufferBindingIndex;

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
        }

        void VertexArray::linkBuffers(const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers)
        {
            if (linkedBuffers.size() != linkedBufferBindingIndices.size()) {
                throw std::runtime_error("Attempting to link an incorrect number of buffers to a vertex array. Expected "
                    + std::to_string(linkedBufferBindingIndices.size()) + " and received " + std::to_string(linkedBuffers.size())
                );
            }

            for (const auto &bufferRef : linkedBuffers) {
                const auto &buffer = bufferRef.get();
                const auto &bufferLayout = buffer.getContentDescription();

                auto bindingIndexIterator = linkedBufferBindingIndices.find(buffer.getName());

                if (bindingIndexIterator == linkedBufferBindingIndices.end()) {
                    throw std::runtime_error("Attempting to link an incompatible buffer to a vertex array.");
                }

                glVertexArrayVertexBuffer(VAO, bindingIndexIterator->second, buffer.getId(), bufferLayout.offset, bufferLayout.stride);

                for (const auto &vertexAttribute : bufferLayout.vertexAttributes) {
                    auto attributeIndex = shaderProgram->getAttribute(vertexAttribute.name);
                    glVertexArrayAttribBinding(VAO, attributeIndex, bindingIndexIterator->second);
                }
            }
        }

        void VertexArray::unbind()
        {
            glBindVertexArray(0);
        }
    }
}
