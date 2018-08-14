#include <chrono>
#include <cmath>
#include <thread>

#include <first-program-tutorial/draw-a-triangle.h>
#include <flare/utility/file.h>

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

        std::vector<uint8_t> vertexShaderSource;
        std::vector<uint8_t> fragmentShaderSource;
        Utility::File::getFileContents(vertexShaderSource, vertexShaderPath);
        Utility::File::getFileContents(fragmentShaderSource, fragmentShaderPath);

        plainTriangleShader = std::make_unique<Flare::Material>(
            vertexShaderSource,
            std::vector<uint8_t>(),
            fragmentShaderSource
        );

        plainTriangleShader->bind();

        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
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
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(renderWindow->getRenderWindowHandle());
    }

    void DrawATriangle::shutdown()
    {
        glDeleteVertexArrays(1, &VAO);
        renderWindow->freeResources();
    }
}
