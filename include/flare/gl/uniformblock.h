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
                return Last::alignment;
            }

            template<typename Current, typename... Remaining>
            constexpr size_t calculateUniformBlockSize(Current current, Remaining... remaining)
            {
                return Current::size + Current::alignment % calculateUniformBlockSize(remaining...) + calculateUniformBlockSize(remaining...);
                // return sizeof(GLSLType) + (sizeof(GLSLType) % glsl_layout_std140_alignment<GLSLType>) + calculateUniformBlockSize<GLSLType, GLSLType...>();
            }

            template<typename... GLSLType>
            class UniformBlock
            {
                private:
                    std::tuple<GLSLType...> blockEntries;

                public:
                    UniformBlock(GLSLType&&... values)
                    {
                        calculateUniformBlockSize(values...);
                    }
            };
        }
    }
}

#endif
