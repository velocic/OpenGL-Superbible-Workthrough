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
            buffersCreated(std::exchange(other.buffersCreated, 0)),
            isCurrentlyEnabled(std::exchange(other.isCurrentlyEnabled, false))
        {
        }

        TransformFeedbackBufferManager &TransformFeedbackBufferManager::operator=(TransformFeedbackBufferManager &&other)
        {
            transformFeedbackBufferMap = std::move(other.transformFeedbackBufferMap);
            buffersCreated = std::exchange(other.buffersCreated, 0);
            isCurrentlyEnabled = std::exchange(other.isCurrentlyEnabled, false);

            return *this;
        }

        const RenderSystem::Buffer *TransformFeedbackBufferManager::create(RenderSystem::ShaderData shaderData, const RenderSystem::VertexDataLayout &bufferLayout, RenderSystem::RSsizei bufferSizeInBytes, RenderSystem::RSsizei count, const std::vector<std::string> &varyings)
        {
            auto buffer = RenderSystem::createBuffer(
                createdBufferBaseName + std::to_string(buffersCreated++),
                bufferLayout
            );

            buffer->allocateBufferStorage(bufferSizeInBytes, nullptr, 0);
            auto result = buffer.get();

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

            return result;
        }

        void TransformFeedbackBufferManager::destroy(size_t hashedAlias)
        {
            auto target = transformFeedbackBufferMap.find(hashedAlias);

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

        void TransformFeedbackBufferManager::destroy(const std::string &alias)
        {
            destroy(stringHasher(alias));
        }

        void TransformFeedbackBufferManager::destroy(RenderSystem::ShaderData shaderData)
        {
            destroy(shaderData.hashedAlias);
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
            for (const auto &it : transformFeedbackBufferMap) {
                glTransformFeedbackVaryings(
                    it.second.linkedShader->getProgramId(),
                    0,
                    nullptr,
                    GL_INTERLEAVED_ATTRIBS
                );

                //TODO: re-link shader
            }

            transformFeedbackBufferMap.clear();
        }

        void TransformFeedbackBufferManager::beginTransformFeedback(RenderSystem::RSenum primitiveMode)
        {
            isCurrentlyEnabled = true;
            glBeginTransformFeedback(primitiveMode);
        }

        void TransformFeedbackBufferManager::endTransformFeedback()
        {
            glEndTransformFeedback();
            isCurrentlyEnabled = false;
        }

        void TransformFeedbackBufferManager::pauseTransformFeedback()
        {
            glPauseTransformFeedback();
        }

        void TransformFeedbackBufferManager::resumeTransformFeedback()
        {
            glResumeTransformFeedback();
        }

        void TransformFeedbackBufferManager::disableRasterization()
        {
            glEnable(GL_RASTERIZER_DISCARD);
        }

        void TransformFeedbackBufferManager::enableRasterization()
        {
            glDisable(GL_RASTERIZER_DISCARD);
        }
    }
}
