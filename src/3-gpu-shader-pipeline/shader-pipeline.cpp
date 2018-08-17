#include <3-gpu-shader-pipeline/shader-pipeline.h>
#include <flare/utility/file.h>

namespace Tutorial
{
    void ShaderPipeline::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Demonstrating the Programmable Pipeline",
            1920,
            1080
        );

        std::vector<uint8_t> vertexShaderSource;
        std::vector<uint8_t> fragmentShaderSource;
        Utility::File::getFileContents(vertexShaderSource, vertexShaderPath);
        Utility::File::getFileContents(fragmentShaderSource, fragmentShaderPath);

        shaderPipelineDemoShader = std::make_unique<Flare::Material>(
            vertexShaderSource,
            std::vector<uint8_t>(),
            fragmentShaderSource
        );

        shaderPipelineDemoShader->bind();

        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }

    void ShaderPipeline::render(unsigned int deltaTime)
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

    void ShaderPipeline::shutdown()
    {
        glDeleteVertexArrays(1, &VAO);
        renderWindow->freeResources();
    }
}
