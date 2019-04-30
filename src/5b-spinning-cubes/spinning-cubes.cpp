#include <5b-spinning-cubes/spinning-cubes.h>
#include <flare/utility/file.h>
#include <glm-0.9.9/glm.hpp>

namespace Tutorial
{
    void SpinningCubes::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Spinning 3D Cubes",
            1280,
            720
        );

        cubeVertexPositions = std::array<GLfloat, 36>{
            -0.25f, 0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            0.25f, -0.25f, -0.25f,

            0.25f, -0.25f, -0.25f,
            0.25f, 0.25f, -0.25f,
            -0.25f, 0.25f, -0.25f,

            -0.25f, 0.25f, -0.25f,
            0.25f, 0.25f, -0.25f,
            0.25f, 0.25f, 0.25f,

            0.25f, 0.25f, 0.25f,
            -0.25f, 0.25f, 0.25f,
            -0.25f, 0.25f, -0.25f
        };
    }

    void SpinningCubes::render(unsigned int deltaTime)
    {
    }

    void SpinningCubes::shutdown()
    {
    }
}
