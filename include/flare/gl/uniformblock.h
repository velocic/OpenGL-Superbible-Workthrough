#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <tuple>
#include <glm-0.9.9/glm.hpp>

namespace Flare
{
    namespace GL
    {
        namespace UBO
        {
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

            template<typename... GLSLType>
            class UniformBlock
            {
                private:
                    const size_t size;
                    // std::tuple<GLSLType...> blockEntries;

                public:
                    UniformBlock(GLSLType&&... values)
                    :
                        size(calculateUniformBlockSize(values...))
                    {}

                    size_t getSize() {return size;}
            };
        }
    }
}

#endif