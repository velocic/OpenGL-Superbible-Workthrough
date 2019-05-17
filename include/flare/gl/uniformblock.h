#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>

#include <iostream>

#include <GL/gl3w.h>

#include <glm-0.9.9/glm.hpp>
#include <flare/gl/uniformblock_priv.h>

namespace Flare
{
    namespace GL
    {
        template<typename T, size_t N>
        struct GLSLArrayType {
            static constexpr size_t size = 16 * N;
            static constexpr size_t alignment = 16;
            using type = T;
        };

        template<typename T>
        struct GLSLType {
            static constexpr size_t size = sizeof(T);
            static constexpr size_t alignment = sizeof(T);
            using type = T;
        };

        template<>
        struct GLSLType<bool> {
            static constexpr size_t size = 4;
            static constexpr size_t alignment = 4;
            using type = bool;
        };

        template<>
        struct GLSLType<glm::vec3> {
            static constexpr size_t size = 12;
            static constexpr size_t alignment = 16;
            using type = glm::vec3;
        };

        template<>
        struct GLSLType<glm::mat4> {
            static constexpr size_t size = 64;
            static constexpr size_t alignment = 16;
            using type = glm::mat4;
        };

        template<typename UniformBlockTupleType>
        class UniformBuffer
        {
            private:
                GLuint boundIndex = 0;
                GLuint glBuffer = 0;
                UniformBlockTupleType uniformBlock;
            public:
                UniformBuffer(UniformBlockTupleType &&uniformBlock)
                :
                    uniformBlock(std::move(uniformBlock))
                {
                    glCreateBuffers(1, &glBuffer);
                }

                ~UniformBuffer()
                {
                    glDeleteBuffers(1, &glBuffer);
                }

                UniformBuffer(UniformBuffer &&other)
                :
                    boundIndex(other.boundIndex),
                    glBuffer(other.glBuffer),
                    uniformBlock(std::move(other.uniformBlock))
                {
                    other.boundIndex = 0;
                    other.glBuffer = 0;
                }

                UniformBuffer &operator=(UniformBuffer &&other)
                {
                    boundIndex = other.boundIndex;
                    glBuffer = other.glBuffer;
                    uniformBlock = std::move(other.uniformBlock);

                    other.boundIndex = 0;
                    other.glBuffer = 0;
                }

                UniformBuffer(const UniformBuffer &other) = delete;
                UniformBuffer &operator=(const UniformBuffer &other) = delete;

                void bindBufferBase(GLuint index)
                {
                    boundIndex = index;
                    glBindBufferBase(GL_UNIFORM_BUFFER, index, glBuffer);
                }

                void unbind()
                {
                    glBindBufferBase(GL_UNIFORM_BUFFER, boundIndex, 0);
                }

                UniformBlockTupleType &getHandles()
                {
                    return uniformBlock;
                }

                void namedBufferData(GLenum usage)
                {
                    glNamedBufferData(
                        glBuffer,
                        std::get<0>(uniformBlock)->size(),
                        std::get<0>(uniformBlock).get()->data(),
                        usage
                    );
                }
        };

        template<typename... GLSLTypes>
        constexpr auto buildStd140AlignedUniformBlockBuffer(GLSLTypes... args)
        {
            constexpr auto bufferSize = HelperTemplates::calculateUniformBlockSize(args...);

            constexpr auto alignedElementIndices = HelperTemplates::calculateUniformBlockAlignedElements(args...);

            auto buffer = std::make_unique<std::array<uint8_t, bufferSize>>();

            constexpr decltype(HelperTemplates::buildEmptyUniformBlockElementPointerTuple(args...)) destinationPointerTuple{};

            auto underlyingBufferAddress = buffer.get();
            auto alignedBlockTuple = std::tuple_cat(
                std::make_tuple(
                    std::move(buffer)
                ),
                HelperTemplates::assignPointersIntoBuffer<bufferSize>(destinationPointerTuple, alignedElementIndices, underlyingBufferAddress)
            );

            return std::make_unique<UniformBuffer<decltype(alignedBlockTuple)>>(std::move(alignedBlockTuple));
        }

    }
}

#endif
