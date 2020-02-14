#ifndef FLARE_RENDERSYSTEM_TRANSFORMFEEDBACKBUFFERMANAGER_H
#define FLARE_RENDERSYSTEM_TRANSFORMFEEDBACKBUFFERMANAGER_H

#include <string>
#include <vector>

#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/datatypes.h>

namespace Flare
{
    namespace RenderSystem
    {
        class TransformFeedbackBufferManager
        {
            public:
                using TransformFeedbackBuffers = std::vector<Buffer *>;

                virtual ~TransformFeedbackBufferManager() {}

                virtual const TransformFeedbackBuffers *create(ShaderData shaderData, RSsizei count, const std::vector<std::string> &varyings, RSenum bufferMode) = 0;
                virtual const TransformFeedbackBuffers *get(const std::string &alias) const = 0;
                virtual const TransformFeedbackBuffers *get(ShaderData shaderData) const = 0;
                virtual void clear() = 0;
                virtual void beginTransformFeedback(RSenum primitiveMode) = 0;
                virtual void endTransformFeedback() = 0;
                virtual void pauseTransformFeedback() = 0;
                virtual void resumeTransformFeedback() = 0;
                virtual void disableRasterization() = 0;
                virtual void enableRasterization() = 0;
        };
    }
}

#endif
