#ifndef FLARE_UTILITY_DATATYPES_H
#define FLARE_UTILITY_DATATYPES_H

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/quaternion.hpp>

#include <flare/rendersystem/buffer.h>

namespace Flare
{
    namespace DataTypes
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uvCoords;
        };

        struct DecomposedModelMatrix {
            glm::vec4 perspective;
            glm::quat orientation;
            glm::vec3 translation;
            glm::vec3 scale;
            glm::vec3 skew;
        };
    }
}

#endif
