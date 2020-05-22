#include <flare/gl/transformfeedbackhandler.h>

namespace Flare
{
    namespace GL
    {
        TransformFeedbackHandler::TransformFeedbackHandler(const RenderSystem::ShaderProgram *shader)
        :
            shader(shader)
        {
        }

        TransformFeedbackHandler::~TransformFeedbackHandler()
        {
        }

        TransformFeedbackHandler::TransformFeedbackHandler(TransformFeedbackHandler &&other)
        {
        }

        TransformFeedbackHandler &TransformFeedbackHandler::operator=(TransformFeedbackHandler &&other)
        {
        }

        void TransformFeedbackHandler::setup(const InterleavedAttributeBufferInitParams &bufferInitParams)
        {
        }

        void TransformFeedbackHandler::setup(const std::vector<SeparatedAttributeBufferInitParams> &bufferInitParams)
        {
        }

        void TransformFeedbackHandler::destroy()
        {
        }

        const RenderSystem::Buffer *TransformFeedbackHandler::get(const std::string &varyingName) const
        {
        }

        void TransformFeedbackHandler::beginTransformFeedback(RenderSystem::RSenum primitiveMode)
        {
        }

        void TransformFeedbackHandler::endTransformFeedback()
        {
        }

        void TransformFeedbackHandler::pauseTransformFeedback()
        {
        }

        void TransformFeedbackHandler::resumeTransformFeedback()
        {
        }
    }
}
