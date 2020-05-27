#ifndef FLARE_GL_TRANSFORMFEEDBACKHANDLER_H
#define FLARE_GL_TRANSFORMFEEDBACKHANDLER_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <flare/rendersystem/doublebuffer.h>
#include <flare/rendersystem/shaderprogram.h>
#include <flare/rendersystem/transformfeedbackhandler.h>

namespace Flare
{
    namespace GL
    {
        // class TransformFeedbackHandler : public RenderSystem::TransformFeedbackHandler
        // {
        //     private:
        //         struct DoubleBuffer {
        //             std::unique_ptr<RenderSystem::Buffer> readBuffer;
        //             std::unique_ptr<RenderSystem::Buffer> writeBuffer;
        //         };
        //
        //         std::unordered_map<size_t, DoubleBuffer> transformFeedbackBuffers;
        //         std::hash<std::string> stringHasher;
        //         RenderSystem::ShaderProgram *shader = nullptr;
        //         bool isCurrentlyEnabled = false;
        //
        //     public:
        //         TransformFeedbackHandler(RenderSystem::ShaderProgram *shader);
        //         virtual ~TransformFeedbackHandler() override;
        //         TransformFeedbackHandler(TransformFeedbackHandler &&other);
        //         TransformFeedbackHandler &operator=(TransformFeedbackHandler &&other);
        //         TransformFeedbackHandler(const TransformFeedbackHandler &other) = delete;
        //         TransformFeedbackHandler &operator=(TransformFeedbackHandler &other) = delete;
        //
        //         virtual void setup(const InterleavedAttributeBufferInitParams &bufferInitParams) override;
        //         virtual void setup(const std::vector<SeparatedAttributeBufferInitParams> &bufferInitParams) override;
        //
        //         virtual void destroy() override;
        //
        //         virtual const RenderSystem::Buffer *get(const std::string &varyingName) const override;
        //
        //         virtual void beginTransformFeedback(RenderSystem::RSenum primitiveMode) override;
        //         virtual void endTransformFeedback() override;
        //         virtual void pauseTransformFeedback() override;
        //         virtual void resumeTransformFeedback() override;
        // };

        class SingleChannelTransformFeedbackHandler : public RenderSystem::SingleChannelTransformFeedbackHandler
        {
            private:
                RenderSystem::DoubleBuffer transformFeedbackBuffer;
                RenderSystem::ShaderProgram *shader = nullptr;
                bool isCurrentlyEnabled = false;

            public:
                SingleChannelTransformFeedbackHandler(RenderSystem::ShaderProgram *shader);
                virtual ~SingleChannelTransformFeedbackHandler() override;
                SingleChannelTransformFeedbackHandler(SingleChannelTransformFeedbackHandler &&other);
                SingleChannelTransformFeedbackHandler &operator=(SingleChannelTransformFeedbackHandler &&other);
                SingleChannelTransformFeedbackHandler(const SingleChannelTransformFeedbackHandler &other) = delete;
                SingleChannelTransformFeedbackHandler &operator=(const SingleChannelTransformFeedbackHandler &other) = delete;

                virtual const RenderSystem::Buffer *get() const override;
                virtual const RenderSystem::Buffer *setup(const InitParams &initParams) override;
                virtual void destroy() override;
                virtual void beginTransformFeedback(RenderSystem::RSenum primitiveMode) override;
                virtual void endTransformFeedback() override;
                virtual void pauseTransformFeedback() override;
                virtual void resumeTransformFeedback() override;
        };
    }
}

#endif
