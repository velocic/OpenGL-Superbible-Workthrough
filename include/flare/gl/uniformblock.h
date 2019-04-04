#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <glm-0.9.9/glm.hpp>

namespace Flare
{
    namespace GL
    {
        namespace UBO
        {

            template<typename GLSLType>
            constexpr size_t glsl_layout_std140_alignment = sizeof(GLSLType);

            template<>
            constexpr size_t glsl_layout_std140_alignment<bool> = 4;

            template<>
            constexpr size_t glsl_layout_std140_alignment<glm::vec3> = 16;

            template<>
            constexpr size_t glsl_layout_std140_alignment<glm::mat2> = 16;

            template<>
            constexpr size_t glsl_layout_std140_alignment<glm::mat3> = 16;

            template<>
            constexpr size_t glsl_layout_std140_alignment<glm::mat4> = 16;

            //layout(std140) alignment of each element of scalar arrays or vector arrays 
            //(which must be of length > 1
            template<typename GLSLType, unsigned int ArraySize>
            constexpr size_t glsl_layout_std140_array_element_alignment = 16;

            //Calculate UBO total size with padding using layout(std140) alignment rules
            template<typename GLSLType>
            constexpr size_t calculateUniformBlockSize()
            {
                return 0;
            };

            template<typename GLSLType, GLSLType...>
            constexpr size_t calculateUniformBlockSize()
            {
                return sizeof(GLSLType) + (sizeof(GLSLType) % glsl_layout_std140_alignment<GLSLType>) + calculateUniformBlockSize<GLSLType, GLSLType...>();
            }

            // template<typename... GLSLType>
            // class UniformBlock
            // {
            //     private:
            //
            //     public:
            //         UniformBlock()
            //         {
            //         }
            //
            //         constexpr size_t getSize()
            //         {
            //         }
            //
            //         constexpr size_t getSize<GLSLType, GLSLType...>()
            //         {
            //         }
            // };
        }
    }
}

#endif
