#include <flare/gl/transformfeedbackhandler.h>

#include <flare/rendersystem/factory.h>
#include <flare/gl/shaderprogram.h>

namespace Flare
{
    namespace GL
    {
        SingleChannelTransformFeedbackHandler::SingleChannelTransformFeedbackHandler(RenderSystem::ShaderProgram *shader)
        :
            shader(shader)
        {
        }

        SingleChannelTransformFeedbackHandler::~SingleChannelTransformFeedbackHandler()
        {
            destroy();
        }

        SingleChannelTransformFeedbackHandler::SingleChannelTransformFeedbackHandler(SingleChannelTransformFeedbackHandler &&other)
        :
            transformFeedbackBuffer(std::exchange(other.transformFeedbackBuffer, RenderSystem::DoubleBuffer{})),
            shader(std::exchange(other.shader, nullptr)),
            isCurrentlyEnabled(std::exchange(other.isCurrentlyEnabled, false))
        {
        }

        SingleChannelTransformFeedbackHandler &SingleChannelTransformFeedbackHandler::operator=(SingleChannelTransformFeedbackHandler &&other)
        {
            transformFeedbackBuffer = std::exchange(other.transformFeedbackBuffer, RenderSystem::DoubleBuffer{});
            shader = std::exchange(other.shader, nullptr);
            isCurrentlyEnabled = std::exchange(other.isCurrentlyEnabled, false);

            return *this;
        }

        const RenderSystem::Buffer *SingleChannelTransformFeedbackHandler::get() const
        {
            return transformFeedbackBuffer.readBuffer.get();
        }

        const RenderSystem::Buffer *SingleChannelTransformFeedbackHandler::setup(const InitParams &initParams)
        {
            transformFeedbackBuffer.readBuffer = RenderSystem::createBuffer(
                "InterleavedTransformFeedbackBuffer",
                initParams.layout,
                RenderSystem::BufferType::TRANSFORMFEEDBACK
            );
            transformFeedbackBuffer.writeBuffer = RenderSystem::createBuffer(
                "InterleavedTransformFeedbackBuffer",
                initParams.layout,
                RenderSystem::BufferType::TRANSFORMFEEDBACK
            );

            transformFeedbackBuffer.readBuffer->allocateBufferStorage(initParams.sizeInBytes, initParams.initialData, initParams.usageFlags);
            transformFeedbackBuffer.writeBuffer->allocateBufferStorage(initParams.sizeInBytes, nullptr, initParams.usageFlags);
            transformFeedbackBuffer.writeBuffer->copyRange(*transformFeedbackBuffer.readBuffer, 0, 0, initParams.sizeInBytes);

            auto CStringVaryings = std::vector<const char *>{};
            CStringVaryings.reserve(initParams.varyings.size());
            std::transform(
                initParams.varyings.begin(),
                initParams.varyings.end(),
                CStringVaryings.begin(),
                [](const auto &inputString){
                    return inputString.c_str();
                }
            );

            glTransformFeedbackVaryings(
                shader->getProgramId(),
                CStringVaryings.size(),
                CStringVaryings.data(),
                GL_INTERLEAVED_ATTRIBS
            );
            static_cast<GL::ShaderProgram *>(shader)->reLink();

            return transformFeedbackBuffer.readBuffer.get();
        }

        void SingleChannelTransformFeedbackHandler::destroy()
        {
            if (shader != nullptr) {
                glTransformFeedbackVaryings(
                    shader->getProgramId(),
                    0,
                    nullptr,
                    GL_INTERLEAVED_ATTRIBS
                );
                static_cast<GL::ShaderProgram *>(shader)->reLink();
            }

            shader = nullptr;
            isCurrentlyEnabled = false;

            transformFeedbackBuffer.destroy();
        }

        void SingleChannelTransformFeedbackHandler::beginTransformFeedback(RenderSystem::RSenum primitiveMode)
        {
            isCurrentlyEnabled = true;

            //TODO: this will bash over other transform feedback buffers. Need a solution using glBindBuffersBase and
            //something external to this class that gives this one a buffer binding index.
            transformFeedbackBuffer.writeBuffer->bind(GL_TRANSFORM_FEEDBACK_BUFFER);
            glBeginTransformFeedback(primitiveMode);
        }

        void SingleChannelTransformFeedbackHandler::endTransformFeedback()
        {
            glEndTransformFeedback();

            transformFeedbackBuffer.swap();
            isCurrentlyEnabled = false;
        }

        void SingleChannelTransformFeedbackHandler::pauseTransformFeedback()
        {
            glPauseTransformFeedback();
        }

        void SingleChannelTransformFeedbackHandler::resumeTransformFeedback()
        {
            glResumeTransformFeedback();
        }
    }
}
