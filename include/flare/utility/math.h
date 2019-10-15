#ifndef FLARE_UTILITY_MATH_H
#define FLARE_UTILITY_MATH_H

#include <glm-0.9.9/glm.hpp>

namespace Flare
{
    namespace Math
    {
        const glm::mat4 identityMatrix = glm::mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        constexpr double PI = 3.1415926535897;

        constexpr double degreesToRadians(double angle)
        {
            return (angle * PI) / 180;
        }
        constexpr double radiansToDegrees(double angle)
        {
            return angle * (180/PI);
        }
    }
}

#endif
