#include <flare/gl/transformfeedbackbuffermanager.h>

namespace Flare
{
    namespace GL
    {
        TransformFeedbackBufferManager::TransformFeedbackBufferManager()
        {
        }

        TransformFeedbackBufferManager::~TransformFeedbackBufferManager()
        {
        }

        TransformFeedbackBufferManager::TransformFeedbackBufferManager(TransformFeedbackBufferManager &&other)
        {
        }

        TransformFeedbackBufferManager &TransformFeedbackBufferManager::operator=(TransformFeedbackBufferManager &&other)
        {
        }

        const RenderSystem::TransformFeedbackBufferManager::TransformFeedbackBuffers *TransformFeedbackBufferManager::create(RenderSystem::ShaderData shaderData, RenderSystem::RSsizei count, const std::vector<std::string> &varyings, RenderSystem::RSenum bufferMode)
        {
            return nullptr;
        }

        const RenderSystem::TransformFeedbackBufferManager::TransformFeedbackBuffers *TransformFeedbackBufferManager::get(const std::string &alias) const
        {
            return nullptr;
        }

        const RenderSystem::TransformFeedbackBufferManager::TransformFeedbackBuffers *TransformFeedbackBufferManager::get(RenderSystem::ShaderData shaderData) const
        {
            return nullptr;
        }

        void TransformFeedbackBufferManager::clear()
        {
        }

        void TransformFeedbackBufferManager::beginTransformFeedback(RenderSystem::RSenum primitiveMode)
        {
        }

        void TransformFeedbackBufferManager::endTransformFeedback()
        {
        }

        void TransformFeedbackBufferManager::pauseTransformFeedback()
        {
        }

        void TransformFeedbackBufferManager::resumeTransformFeedback()
        {
        }

        void TransformFeedbackBufferManager::disableRasterization()
        {
        }

        void TransformFeedbackBufferManager::enableRasterization()
        {
        }
    }
}
