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
            auto buffers = DoubleBuffer{};
            buffers.readBuffer = RenderSystem::createBuffer(
                createdReadBufferBaseName + std::to_string(buffersCreated),
                bufferLayout,
                RenderSystem::BufferType::TRANSFORMFEEDBACK
            );
            buffers.readBuffer->allocateBufferStorage(bufferSizeInBytes, initialData, bufferUsageFlags);

            buffers.writeBuffer = RenderSystem::createBuffer(
                createdWriteBufferBaseName + std::to_string(buffersCreated++),
                bufferLayout,
                RenderSystem::BufferType::TRANSFORMFEEDBACK
            );
            buffers.writeBuffer->allocateBufferStorage(bufferSizeInBytes, nullptr, bufferUsageFlags);
            buffers.writeBuffer->copyRange(*buffers.readBuffer, 0, 0, bufferSizeInBytes);

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

            auto result = buffers.readBuffer.get();

            transformFeedbackBufferMap.insert_or_assign(
                shaderData.hashedAlias,
                ShaderToDoubleBuffer{
                    std::move(buffers),
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
                return result->second.transformFeedbackBuffers.readBuffer.get();
            }

            return nullptr;
        }

        const RenderSystem::Buffer *TransformFeedbackBufferManager::get(RenderSystem::ShaderData shaderData) const
        {
            auto result = transformFeedbackBufferMap.find(shaderData.hashedAlias);

            if (result != transformFeedbackBufferMap.end()) {
                return result->second.transformFeedbackBuffers.readBuffer.get();
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
                bufferNames.push_back(value.transformFeedbackBuffers.writeBuffer->getId());
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

            for (auto &[key, value] : transformFeedbackBufferMap) {
                auto &buffers = value.transformFeedbackBuffers;
                buffers.readBuffer.swap(buffers.writeBuffer);
            }

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
