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
                virtual ~TransformFeedbackBufferManager() {}

                virtual const Buffer *create(ShaderData shaderData, const VertexDataLayout &bufferLayout, RSsizei bufferSizeInBytes, RSsizei count, const std::vector<std::string> &varyings) = 0;
                virtual void destroy(size_t hashedAlias) = 0;
                virtual void destroy(const std::string &alias) = 0;
                virtual void destroy(ShaderData shaderData) = 0;
                virtual const Buffer *get(const std::string &alias) const = 0;
                virtual const Buffer *get(ShaderData shaderData) const = 0;
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
