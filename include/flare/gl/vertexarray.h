#ifndef FLARE_GL_VERTEXARRAY_H
#define FLARE_GL_VERTEXARRAY_H

#include <functional>
#include <string>
#include <unordered_map>

#include <GL/gl3w.h>
#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/shaderprogram.h>
#include <flare/rendersystem/vertexarray.h>

namespace Flare
{
    namespace GL
    {
        class VertexArray : public RenderSystem::VertexArray
        {
            private:
                const RenderSystem::ShaderProgram *shaderProgram;
                std::unordered_map<size_t, RenderSystem::RSuint> linkedBufferBindingIndices;
                GLuint VAO = 0;

                void configureAttributesFromInitialBuffers(GLuint VAO, const RenderSystem::ShaderProgram& shaderProgram, const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers);
            public:
                VertexArray(const RenderSystem::ShaderProgram *shaderProgram, const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers);
                VertexArray(VertexArray &&other);
                VertexArray &operator=(VertexArray &&other);
                VertexArray(const VertexArray &other) = delete;
                VertexArray &operator=(const VertexArray &other) = delete;
                virtual ~VertexArray();

                virtual void bind() override;
                virtual void destroy() override;
                virtual void linkBuffers(const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers) override;
                virtual void unbind() override;
        };
    }
}

#endif
