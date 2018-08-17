#include <3-gpu-shader-pipeline/shader-pipeline.h>
#include <flare/utility/file.h>
#include <iostream>

namespace Tutorial
{
    void ShaderPipeline::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Demonstrating the Programmable Pipeline",
            1280,
            720
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
        shaderPipelineDemoShader->addAttribute(VAO, "offset");
    }

    void ShaderPipeline::render(unsigned int deltaTime)
    {
        elapsedTime += deltaTime;

        const GLfloat clearColor[] = {
            (float)sin(elapsedTime) * 0.5f + 0.5f,
            (float)cos(elapsedTime) * 0.5f + 0.5f,
            0.0f,
            1.0f
        };

        GLfloat vertexOffset[] = {
            (float)sin(elapsedTime) * 0.5f,
            (float)cos(elapsedTime) * 0.6f,
            0.0f,
            0.0f
        };

        //update the "offset" attribute of the vertex shader, with
        //vertexOffset on the currently attached vertex array object
        glBindVertexArray(VAO);
        glVertexAttrib4fv(shaderPipelineDemoShader->getAttribute(VAO, "offset"), vertexOffset);
        
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
