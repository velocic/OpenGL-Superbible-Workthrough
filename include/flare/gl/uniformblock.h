#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <memory>
#include <tuple>
#include <utility>

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
                using underlying_type = T*;
            };

            template<typename T>
            struct GLSLType {
                static constexpr size_t size = sizeof(T);
                static constexpr size_t alignment = sizeof(T);
                using underlying_type = T;
            };

            template<>
            struct GLSLType<bool> {
                static constexpr size_t size = 4;
                static constexpr size_t alignment = 4;
                using underlying_type = bool;
            };

            template<>
            struct GLSLType<glm::vec3> {
                static constexpr size_t size = 12;
                static constexpr size_t alignment = 16;
                using underlying_type = glm::vec3;
            };

            template<>
            struct GLSLType<glm::mat4> {
                static constexpr size_t size = 64;
                static constexpr size_t alignment = 16;
                using underlying_type = glm::mat4;
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
            class UniformBlockMaker
            {
                public:
                    using BlockEntryPointersType = std::tuple<>;

                    BlockEntryPointersType blockEntryPointers;
            };

            template<typename GLSLType, typename... GLSLTypes>
            class UniformBlockMaker<GLSLType, GLSLTypes...>
            {
                public:
                    using TailBlockEntryPointersType = typename UniformBlockMaker<GLSLTypes...>::BlockEntryPointersType;
                    using BlockEntryPointersType = decltype(
                        std::tuple_cat(
                            std::tuple<typename GLSLType::underlying_type*>(),
                            std::declval<TailBlockEntryPointersType>()
                        )
                    );

                    BlockEntryPointersType blockEntryPointers;

                    UniformBlockMaker(GLSLType first, GLSLTypes... rest)
                    :
                        blockEntryPointers(
                            std::tuple_cat(
                                std::tuple<typename GLSLType::underlying_type*>(),
                                UniformBlockMaker<GLSLTypes...>(rest...).blockEntryPointers
                            )
                        )
                    {
                    }

            };

            template<typename... GLSLType>
            class UniformBlock
            {
                private:
                    const size_t size = 0;
                    std::unique_ptr<uint8_t[]> alignedBuffer = nullptr;
                    std::array<size_t, sizeof...(GLSLType)> bufferOffsets;

                public:
                    UniformBlock(GLSLType&&... values)
                    :
                        size(calculateUniformBlockSize(values...)),
                        alignedBuffer(std::make_unique<uint8_t[]>(calculateUniformBlockSize(values...)))
                    {

                        auto testBlockMaker = UniformBlockMaker<GLSLType...>(values...);
                        int debug = 5;
                        // size_t it = 0;
                        // std::apply(
                        //     [&](auto&&... args) {
                        //         ((bufferOffsets[it++] = args), ...);
                        //     },
                        //     calculateUniformBlockAlignedElements(values...)
                        // );
                        //
                        // int debug = 5;
                    }

                    uint8_t *getData() const {return alignedBuffer.get();}
                    size_t getSize() const {return size;}
            };
        }
    }
}

#endif
