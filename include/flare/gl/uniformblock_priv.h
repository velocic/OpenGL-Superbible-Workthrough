#ifndef FLARE_GL_UNIFORMBLOCK_PRIV_H
#define FLARE_GL_UNIFORMBLOCK_PRIV_H

namespace Flare
{
    namespace GL
    {
        namespace HelperTemplates
        {
            template<size_t TotalBlockSize>
            constexpr size_t calculateUniformBlockSize()
            {
                return TotalBlockSize;
            }

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

            template<size_t CurrentElementIndex>
            constexpr auto calculateUniformBlockAlignedElements()
            {
                return std::make_tuple();
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

            template<typename... Pointers, typename... BufferOffsets>
            constexpr auto assignPointersIntoBuffer(const std::tuple<Pointers...> &pointerTuple, const std::tuple<BufferOffsets...> &bufferOffsetsTuple, uint8_t *buffer)
            {
                if constexpr (sizeof...(Pointers) != sizeof...(BufferOffsets)) {
                    return pointerTuple;
                }

                return std::apply(
                    [&](const auto... pointers) {
                        return std::apply(
                            [&](const auto... bufferOffsets) {
                                return std::tuple(reinterpret_cast<decltype(pointers)>(&buffer[bufferOffsets])...);
                            },
                            bufferOffsetsTuple
                        );
                    },
                    pointerTuple
                );
            }
        }
    }
}

#endif
