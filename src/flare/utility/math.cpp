#include <flare/utility/math.h>

namespace Flare
{
    namespace Math
    {
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
