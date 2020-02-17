#include <flare/gl/transformfeedbackbuffermanager.h>

#include <flare/rendersystem/factory.h>

namespace Flare
{
    namespace GL
    {
        TransformFeedbackBufferManager::~TransformFeedbackBufferManager()
        {
            clear();
        }

        TransformFeedbackBufferManager::TransformFeedbackBufferManager(TransformFeedbackBufferManager &&other)
        :
            transformFeedbackBufferMap(std::move(other.transformFeedbackBufferMap)),
            isCurrentlyEnabled(std::exchange(other.isCurrentlyEnabled, false)),
            buffersCreated(std::exchange(other.buffersCreated, 0))
        {
        }

        TransformFeedbackBufferManager &TransformFeedbackBufferManager::operator=(TransformFeedbackBufferManager &&other)
        {
            transformFeedbackBufferMap = std::move(other.transformFeedbackBufferMap);
            isCurrentlyEnabled = std::exchange(other.isCurrentlyEnabled, false);
            buffersCreated = std::exchange(other.buffersCreated, 0);

            return *this;
        }

        const RenderSystem::Buffer *TransformFeedbackBufferManager::create(RenderSystem::ShaderData shaderData, const RenderSystem::VertexDataLayout &bufferLayout, RenderSystem::RSsizei count, const std::vector<std::string> &varyings)
        {
            auto buffer = RenderSystem::createBuffer(
                createdBufferBaseName + std::to_string(buffersCreated++),
                bufferLayout
            );

            //TODO: allocate storage in buffer + set GL_DYNAMIC_COPY

            auto CStringVaryings = std::vector<const char *>{};
            CStringVaryings.reserve(varyings.size());
            std::transform(
                varyings.begin(),
                varyings.end(),
                CStringVaryings.begin(),
                [](const auto &inputString){
                    return inputString.c_str();
                }
            );

            glTransformFeedbackVaryings(
                shaderData.shader->getProgramId(),
                count,
                CStringVaryings.data(),
                GL_INTERLEAVED_ATTRIBS
            );

            transformFeedbackBufferMap.insert_or_assign(
                shaderData.hashedAlias,
                ShaderToOutputBuffer{
                    std::move(buffer),
                    shaderData.shader
                }
            );

            //TODO: re-link shader
        }

        void TransformFeedbackBufferManager::destroy(const std::string &alias)
        {
            auto target = transformFeedbackBufferMap.find(stringHasher(alias));

            if (target != transformFeedbackBufferMap.end()) {
                glTransformFeedbackVaryings(
                    target->second.linkedShader->getProgramId(),
                    0,
                    nullptr,
                    GL_INTERLEAVED_ATTRIBS
                );
                //TODO: re-link shader

                transformFeedbackBufferMap.erase(target);
            }
        }

        void TransformFeedbackBufferManager::destroy(RenderSystem::ShaderData shaderData)
        {
            auto target = transformFeedbackBufferMap.find(shaderData.hashedAlias);

            if (target != transformFeedbackBufferMap.end()) {
                glTransformFeedbackVaryings(
                    target->second.linkedShader->getProgramId(),
                    0,
                    nullptr,
                    GL_INTERLEAVED_ATTRIBS
                );
                //TODO: re-link shader

                transformFeedbackBufferMap.erase(target);
            }
        }

        const RenderSystem::Buffer *TransformFeedbackBufferManager::get(const std::string &alias) const
        {
            auto result = transformFeedbackBufferMap.find(stringHasher(alias));

            if (result != transformFeedbackBufferMap.end()) {
                return result->second.transformFeedbackBuffer.get();
            }

            return nullptr;
        }

        const RenderSystem::Buffer *TransformFeedbackBufferManager::get(RenderSystem::ShaderData shaderData) const
        {
            auto result = transformFeedbackBufferMap.find(shaderData.hashedAlias);

            if (result != transformFeedbackBufferMap.end()) {
                return result->second.transformFeedbackBuffer.get();
            }

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
