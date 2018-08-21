#include <3-gpu-shader-pipeline/shader-pipeline.h>
#include <flare/utility/file.h>
#include <glm-0.9.9/glm.hpp>

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

        auto vertexShaderSource = Utility::File::getFileContents(vertexShaderPath);
        auto fragmentShaderSource = Utility::File::getFileContents(fragmentShaderPath);
        auto tessellationControlShaderSource = Utility::File::getFileContents(tessellationControlShaderPath);
        auto tessellationEvaluationShaderSource = Utility::File::getFileContents(tessellationEvaluationShaderPath);

        shaderPipelineDemoShader = Flare::MaterialBuilder()
            .addVertexShader(std::move(vertexShaderSource))
            .addFragmentShader(std::move(fragmentShaderSource))
            .addTessellationControlShader(std::move(tessellationControlShaderSource))
            .addTessellationEvaluationShader(std::move(tessellationEvaluationShaderSource))
            .build();

        shaderPipelineDemoShader->bind();

        //Number of vertices to use per patch for the tessellation control shader.
        //3 is the default, so this call is unnecessary but good for demonstration.
        glPatchParameteri(GL_PATCH_VERTICES, 3);

        //Wireframe rendering to view tessellated triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glCreateVertexArrays(1, &VAO);
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
        GLfloat triangleColor[] = {1.0f, 0.5f, 0.25f, 1.0f};

        //update the "offset" attribute of the vertex shader, with
        //vertexOffset on the currently attached vertex array object
        glBindVertexArray(VAO);
        
        //attribute location for "offset"
        glVertexAttrib4fv(0, vertexOffset);
        
        //attribute location for "color"
        glVertexAttrib4fv(1, triangleColor);
        
        glClearBufferfv(GL_COLOR, 0, clearColor);

        //Have to draw GL_PATCHES instead of GL_TRIANGLES due to using the tessellation
        //shaders. GL_TRIANGLES renders nothing.
        glDrawArrays(GL_PATCHES, 0, 3);
        SDL_GL_SwapWindow(renderWindow->getRenderWindowHandle());
    }

    void ShaderPipeline::shutdown()
    {
        glDeleteVertexArrays(1, &VAO);
        renderWindow->freeResources();
    }
}
