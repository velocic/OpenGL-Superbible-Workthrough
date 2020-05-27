#ifndef FLARE_RENDERSYSTEM_TRANSFORMFEEDBACKHANDLER_H
#define FLARE_RENDERSYSTEM_TRANSFORMFEEDBACKHANDLER_H

#include <string>
#include <vector>

#include <flare/rendersystem/buffer.h>
#include <flare/rendersystem/datalayout.h>

namespace Flare
{
    namespace RenderSystem
    {
        class TransformFeedbackHandler
        {
            public:
                virtual ~TransformFeedbackHandler() {}

                virtual void destroy() = 0;

                virtual void beginTransformFeedback(RenderSystem::RSenum primitiveMode) = 0;
                virtual void endTransformFeedback() = 0;
                virtual void pauseTransformFeedback() = 0;
                virtual void resumeTransformFeedback() = 0;

                //TODO: these should be free functions in the Flare::Rendersystem namespace
                // virtual void disableRasterization() = 0;
                // virtual void enableRasterization() = 0;
        };

        //All transform feedback varyings are interleaved within a single buffer
        class SingleChannelTransformFeedbackHandler : public TransformFeedbackHandler
        {
            public:
                struct InitParams {
                    std::vector<std::string> varyings;
                    VertexDataLayout layout;
                    RSsizei sizeInBytes;
                    RSbitfield usageFlags;
                    void *initialData = nullptr;
                };

                virtual ~SingleChannelTransformFeedbackHandler() {}

                virtual const Buffer *get() const = 0;
                virtual const Buffer *setup(const InitParams &initParams) = 0;
        };

        //Each transform feedback varying is written to a separate buffer
        class MultiChannelTransformFeedbackHandler : public TransformFeedbackHandler
        {
            public:
                struct InitParams {
                    std::string varying;
                    VertexDataLayout layout;
                    RSsizei sizeInBytes;
                    RSbitfield usageFlags;
                    void *initialData = nullptr;
                };

                virtual ~MultiChannelTransformFeedbackHandler() {}

                virtual std::unordered_map<size_t, const Buffer *> get() const = 0;
                virtual std::unordered_map<size_t, const Buffer *> setup(const std::vector<InitParams> &initParams) = 0;
        };
    }
}

#endif
