#ifndef FLARE_GL_TRANSFORMFEEDBACKBUFFERMANAGER_H
#define FLARE_GL_TRANSFORMFEEDBACKBUFFERMANAGER_H

#include <flare/rendersystem/transformfeedbackbuffermanager.h>
#include <flare/rendersystem/shadermanager.h>

namespace Flare
{
    namespace GL
    {
        class TransformFeedbackBufferManager : public RenderSystem::TransformFeedbackBufferManager
        {
            private:
                struct ShaderToOutputBuffer {
                    std::unique_ptr<RenderSystem::Buffer> transformFeedbackBuffer;
                    RenderSystem::ShaderProgram *linkedShader = nullptr;
                };

                std::unordered_map<size_t, ShaderToOutputBuffer> transformFeedbackBufferMap;
                std::hash<std::string> stringHasher;
                const std::string createdBufferBaseName = "transformFeedbackBuffer";
                size_t buffersCreated = 0;
                bool isCurrentlyEnabled = false;

            public:
                TransformFeedbackBufferManager() {};
                virtual ~TransformFeedbackBufferManager() override;
                TransformFeedbackBufferManager(TransformFeedbackBufferManager &&other);
                TransformFeedbackBufferManager &operator=(TransformFeedbackBufferManager &&other);
                TransformFeedbackBufferManager(const TransformFeedbackBufferManager &other) = delete;
                TransformFeedbackBufferManager &operator=(const TransformFeedbackBufferManager &other) = delete;

                virtual const RenderSystem::Buffer *create(RenderSystem::ShaderData shaderData, const RenderSystem::VertexDataLayout &bufferLayout, RenderSystem::RSsizei bufferSizeInBytes, RenderSystem::RSsizei count, const std::vector<std::string> &varyings) override;
                virtual void destroy(size_t hashedAlias) override;
                virtual void destroy(const std::string &alias) override;
                virtual void destroy(RenderSystem::ShaderData shaderData) override;
                virtual const RenderSystem::Buffer *get(const std::string &alias) const override;
                virtual const RenderSystem::Buffer *get(RenderSystem::ShaderData shaderData) const override;
                virtual void clear() override;
                virtual void beginTransformFeedback(RenderSystem::RSenum primitiveMode) override;
                virtual void endTransformFeedback() override;
                virtual void pauseTransformFeedback() override;
                virtual void resumeTransformFeedback() override;
                virtual void disableRasterization() override;
                virtual void enableRasterization() override;
        };
    }
}

#endif
