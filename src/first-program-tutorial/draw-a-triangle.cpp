#include <chrono>
#include <cmath>
#include <thread>

#include <first-program-tutorial/draw-a-triangle.h>

namespace Tutorial
{
    DrawATriangle::DrawATriangle()
    :
        vertexShaderPath("../src/first-program-tutorial/shaders/vert.vsh"),
        fragmentShaderPath("../src/first-program-tutorial/shaders/frag.fsh")
    {
    }

    void DrawATriangle::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "First Triangle Tutorial",
            1920,
            1080
        );
    }

    void DrawATriangle::render(unsigned int deltaTime)
    {
        const GLfloat clearColor[] = {
            (float)sin(deltaTime) * 0.5f + 0.5f,
            (float)cos(deltaTime) * 0.5f + 0.5f,
            0.0f,
            1.0f
        };

        glClearBufferfv(GL_COLOR, 0, clearColor);
        SDL_GL_SwapWindow(renderWindow->getRenderWindowHandle());
    }

    void DrawATriangle::shutdown()
    {
        renderWindow->freeResources();
    }
}
