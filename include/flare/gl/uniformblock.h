#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <memory>
#include <tuple>

#include <glm-0.9.9/glm.hpp>

namespace Flare
{
    namespace GL
    {
        namespace UBO
        {
            template<typename T, size_t N>
            struct GLSLArrayType {
                static constexpr size_t size = 16 * N;
                static constexpr size_t alignment = 16;
            };

            template<typename T>
            struct GLSLType {
                static constexpr size_t size = sizeof(T);
                static constexpr size_t alignment = sizeof(T);
            };

            template<>
            struct GLSLType<bool> {
                static constexpr size_t size = 4;
                static constexpr size_t alignment = 4;
            };

            template<>
            struct GLSLType<glm::vec3> {
                static constexpr size_t size = 12;
                static constexpr size_t alignment = 16;
            };

            template<>
            struct GLSLType<glm::mat4> {
                static constexpr size_t size = 64;
                static constexpr size_t alignment = 16;
            };

            template<size_t TotalBlockSize, typename Last>
            constexpr size_t calculateUniformBlockSize(Last last)
            {
                if constexpr (TotalBlockSize % Last::alignment == 0) {
                    return Last::size + TotalBlockSize;
                }

                return TotalBlockSize + Last::size + (Last::alignment - (TotalBlockSize % Last::alignment));
            }

            template<size_t TotalBlockSize = 0, typename Current, typename... Remaining>
            constexpr size_t calculateUniformBlockSize(Current current, Remaining... remaining)
            {
                if constexpr (TotalBlockSize % Current::alignment == 0) {
                    constexpr auto currentSize = Current::size + TotalBlockSize;
                    return calculateUniformBlockSize<currentSize>(remaining...);
                }

                constexpr auto paddedCurrentSize = TotalBlockSize + Current::size + (Current::alignment - (TotalBlockSize % Current::alignment));

                return calculateUniformBlockSize<paddedCurrentSize>(remaining...);
            }

            template<size_t CurrentElementIndex, typename Last>
            constexpr auto calculateUniformBlockAlignedElements(Last last)
            {
                if constexpr (CurrentElementIndex % Last::alignment == 0) {
                    return std::make_tuple(CurrentElementIndex);
                }

                constexpr auto alignmentPadding = Last::alignment - (CurrentElementIndex % Last::alignment);

                return std::make_tuple(CurrentElementIndex + alignmentPadding);
            }

            template<size_t CurrentElementIndex = 0, typename Current, typename... Remaining>
            constexpr auto calculateUniformBlockAlignedElements(Current current, Remaining... remaining)
            {
                if constexpr (CurrentElementIndex % Current::alignment == 0) {
                    constexpr auto nextElementCandidateIndex = CurrentElementIndex + Current::size;
                    return std::tuple_cat(std::make_tuple(CurrentElementIndex), calculateUniformBlockAlignedElements<nextElementCandidateIndex>(remaining...));
                }

                constexpr auto alignmentPadding = Current::alignment - (CurrentElementIndex % Current::alignment);
                constexpr auto currentElementAlignedIndex = CurrentElementIndex + alignmentPadding;
                constexpr auto nextElementCandidateIndex = currentElementAlignedIndex + Current::size;

                return std::tuple_cat(std::make_tuple(currentElementAlignedIndex), calculateUniformBlockAlignedElements<nextElementCandidateIndex>(remaining...));
            }

            template<typename... GLSLType>
            class UniformBlock
            {
                private:
                    const size_t size;
                    std::unique_ptr<uint8_t[]> alignedBuffer;
                    std::array<size_t, sizeof...(GLSLType)> bufferOffsets;

                public:
                    UniformBlock(GLSLType&&... values)
                    :
                        size(calculateUniformBlockSize(values...)),
                        alignedBuffer(std::make_unique<uint8_t[]>(calculateUniformBlockSize(values...)))
                    {
                        size_t it = 0;
                        std::apply(
                            [&](auto&&... args) {
                                ((bufferOffsets[it++] = args), ...);
                            },
                            calculateUniformBlockAlignedElements(values...)
                        );
                    }

                    uint8_t *getData() const {return alignedBuffer.get();}
                    size_t getSize() const {return size;}
            };
        }
    }
}

#endif
