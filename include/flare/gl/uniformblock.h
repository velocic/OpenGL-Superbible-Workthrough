#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

#include <cstdlib>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>

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

        template<typename... GLSLTypes>
        constexpr auto buildStd140AlignedUniformBlockBuffer(GLSLTypes... args)
        {
            constexpr auto bufferSize = HelperTemplates::calculateUniformBlockSize(args...);
            constexpr auto alignedElementIndices = HelperTemplates::calculateUniformBlockAlignedElements(args...);

            auto buffer = std::make_unique<uint8_t[]>(bufferSize);

            constexpr decltype(HelperTemplates::buildEmptyUniformBlockElementPointerTuple(args...)) destinationPointerTuple{};

            return std::tuple_cat(std::make_tuple(std::move(buffer)), HelperTemplates::assignPointersIntoBuffer(destinationPointerTuple, alignedElementIndices, buffer.get()));
        }
    }
}

#endif
