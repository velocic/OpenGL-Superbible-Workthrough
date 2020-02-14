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
                using TransformFeedbackBuffers = std::vector<std::unique_ptr<RenderSystem::Buffer>>;

                std::unordered_map<size_t, TransformFeedbackBuffers> transformFeedbackBufferMap;
                std::hash<std::string> stringHasher;
                bool isCurrentlyEnabled = false;

            public:
                TransformFeedbackBufferManager();
                virtual ~TransformFeedbackBufferManager() override;
                TransformFeedbackBufferManager(TransformFeedbackBufferManager &&other);
                TransformFeedbackBufferManager &operator=(TransformFeedbackBufferManager &&other);
                TransformFeedbackBufferManager(const TransformFeedbackBufferManager &other) = delete;
                TransformFeedbackBufferManager &operator=(const TransformFeedbackBufferManager &other) = delete;

                virtual const RenderSystem::TransformFeedbackBufferManager::TransformFeedbackBuffers *create(RenderSystem::ShaderData shaderData, RenderSystem::RSsizei count, const std::vector<std::string> &varyings, RenderSystem::RSenum bufferMode) override;
                virtual const RenderSystem::TransformFeedbackBufferManager::TransformFeedbackBuffers *get(const std::string &alias) const override;
                virtual const RenderSystem::TransformFeedbackBufferManager::TransformFeedbackBuffers *get(RenderSystem::ShaderData shaderData) const override;
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
