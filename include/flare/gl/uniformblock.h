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

            template<typename Last>
            constexpr size_t calculateUniformBlockSize(Last last)
            {
                return Last::size;
            }

            template<typename Current, typename... Remaining>
            constexpr size_t calculateUniformBlockSize(Current current, Remaining... remaining)
            {
                constexpr auto totalSize = calculateUniformBlockSize(remaining...);

                if constexpr (totalSize % Current::alignment == 0) {
                    return totalSize + Current::size;
                }

                return totalSize + Current::size + (Current::alignment - (totalSize % Current::alignment));
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
