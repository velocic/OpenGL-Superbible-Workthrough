#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <glm-0.9.9/glm.hpp>

namespace Flare
{
    namespace GL
    {
        template<typename GLSLType>
        constexpr size_t glsl_size_bytes = sizeof(GLSLType);

        template<typename GLSLType, unsigned int ArraySize>
        constexpr size_t glsl_layout_std140_alignment = sizeof(GLSLType) * ArraySize;

        // template<typename GLSLType>
        // constexpr size_t glsl_layout_std140_alignment = sizeof(GLSLType);
        //
        // template<>
        // constexpr size_t glsl_layout_std140_alignment<glm::vec2> = 8;
        //
        // template<>
        // constexpr size_t glsl_layout_std140_alignment<glm::vec3> = 16;
        //
        // template<>
        // constexpr size_t glsl_layout_std140_alignment<glm::vec4> = 16;
        //
        // template<>
        // constexpr size_t glsl_layout_std140_alignment<glm::mat2> = 4;
        //
        // template<>
        // constexpr size_t glsl_layout_std140_aligment<glm::mat3> = 4;
        //
        // template<>
        // constexpr size_t glsl_layout_std140_alignment<glm::mat4> = 4;

        template<typename... GLSLType>
        class UniformBlock
        {
        };
    }
}

#endif
