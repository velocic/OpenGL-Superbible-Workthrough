#include <flare/gl/transformfeedbackbuffermanager.h>

#include <flare/gl/shaderprogram.h>
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

        const RenderSystem::Buffer *TransformFeedbackBufferManager::create(RenderSystem::ShaderData shaderData, const RenderSystem::VertexDataLayout &bufferLayout, RenderSystem::RSsizei bufferSizeInBytes, RenderSystem::RSbitfield bufferUsageFlags, const std::vector<std::string> &varyings, void *initialData)
        {
            auto buffer = RenderSystem::createBuffer(
                createdBufferBaseName + std::to_string(buffersCreated++),
                bufferLayout,
                RenderSystem::BufferType::TRANSFORMFEEDBACK
            );

            buffer->allocateBufferStorage(bufferSizeInBytes, initialData, bufferUsageFlags);
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
                varyings.size(),
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

            static_cast<GL::ShaderProgram *>(shaderData.shader)->reLink();

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

                static_cast<GL::ShaderProgram *>(target->second.linkedShader)->reLink();

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

                static_cast<GL::ShaderProgram *>(it.second.linkedShader)->reLink();
            }

            transformFeedbackBufferMap.clear();
        }

        void TransformFeedbackBufferManager::beginTransformFeedback(RenderSystem::RSenum primitiveMode)
        {
            isCurrentlyEnabled = true;

            auto bufferNames = std::vector<GLuint>{};
            for (const auto &[key, value] : transformFeedbackBufferMap) {
                bufferNames.push_back(value.transformFeedbackBuffer->getId());
            }

            glBindBuffersBase(
                GL_TRANSFORM_FEEDBACK_BUFFER,
                0,
                transformFeedbackBufferMap.size(),
                bufferNames.data()
            );
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
