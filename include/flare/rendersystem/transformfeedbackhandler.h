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
                struct InterleavedAttributeBufferInitParams {
                    std::vector<std::string> varyings;
                    VertexDataLayout layout;
                    RSsizei sizeInBytes;
                    RSbitfield usageFlags;
                    void *initialData = nullptr;
                };

                struct SeparatedAttributeBufferInitParams {
                    std::string varying;
                    VertexDataLayout layout;
                    RSsizei sizeInBytes;
                    RSbitfield usageFlags;
                    void *initialData = nullptr;
                };

                virtual ~TransformFeedbackHandler() {}

                virtual void setup(const InterleavedAttributeBufferInitParams &bufferInitParams) = 0;
                virtual void setup(const std::vector<SeparatedAttributeBufferInitParams> &bufferInitParams) = 0;

                virtual void destroy() = 0;

                virtual const Buffer *get(const std::string &varyingName) const = 0;

                virtual void beginTransformFeedback(RenderSystem::RSenum primitiveMode) = 0;
                virtual void endTransformFeedback() = 0;
                virtual void pauseTransformFeedback() = 0;
                virtual void resumeTransformFeedback() = 0;

                //TODO: these should be free functions in the Flare::Rendersystem namespace
                // virtual void disableRasterization() = 0;
                // virtual void enableRasterization() = 0;
        };
    }
}

#endif
