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

        auto vertexShaderSource = Utility::File::getFileContents(vertexShaderPath);
        auto fragmentShaderSource = Utility::File::getFileContents(fragmentShaderPath);

        plainTriangleShader = Flare::GL::ShaderProgramBuilder()
            .addVertexShader(std::move(vertexShaderSource))
            .addFragmentShader(std::move(fragmentShaderSource))
            .build();

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
        renderWindow->swapWindow();
    }

    void DrawATriangle::shutdown()
    {
        glDeleteVertexArrays(1, &VAO);
        renderWindow->freeResources();
    }
}
