#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <functional>
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

            template<typename GLSLType>
            constexpr auto buildEmptyUniformBlockElementPointerTuple(GLSLType last)
            {
                return std::tuple<typename GLSLType::type*>();
            }

            template<typename GLSLType, typename... GLSLTypes>
            constexpr auto buildEmptyUniformBlockElementPointerTuple(GLSLType current, GLSLTypes... rest)
            {
                return std::tuple_cat(std::tuple<typename GLSLType::type*>(), buildEmptyUniformBlockElementPointerTuple(rest...));
            }

            template<typename... FirstTuplePack, typename... SecondTuplePack>
            constexpr auto zipTuples(const std::tuple<FirstTuplePack...> &t1, const std::tuple<SecondTuplePack...> &t2)
            {
                return std::apply(
                    [&](const auto &... t1s) {
                        return std::apply(
                            [&](const auto &... t2s) {
                                return std::tuple(std::pair(t1s, t2s)...);
                            },
                            t2
                        );
                    },
                    t1
                );
                // return zipTuples(std::make_integer_sequence<size_t, std::tuple_size<FirstTuple>::value>(), t1, t2);
            }

            template<typename... GLSLTypes>
            constexpr auto buildAlignedUniformBlockBuffer(GLSLTypes... args)
            {
                constexpr auto bufferSize = calculateUniformBlockSize(args...);
                constexpr auto alignedElementIndices = calculateUniformBlockAlignedElements(args...);

                auto buffer = std::make_unique<uint8_t[]>(bufferSize);

                constexpr decltype(buildEmptyUniformBlockElementPointerTuple(args...)) destinationPointerTuple{};

                // std::apply(
                //     [&](auto&&... elementPointers) {
                //         destinationPointerTuple = std::tuple_cat();
                //         // ((elementPointers = reinterpret_cast<decltype(elementPointers)>(buffer.get()[elementIndexArray[elementIndex++]])), ...);
                //     },
                //     bufferElementPointers
                // );
                
                // constexpr auto test = zipTuples(getIndexSequenceForTuple(alignedElementIndices), alignedElementIndices, destinationPointerTuple);
                constexpr auto test = zipTuples(alignedElementIndices, destinationPointerTuple);

                return std::tuple_cat(std::make_tuple(std::move(buffer)), std::move(destinationPointerTuple));
                // return destinationPointerTuple;
            }
        }
    }
}

#endif
