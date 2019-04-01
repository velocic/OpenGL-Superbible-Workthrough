#include <5-data/data.h>
#include <flare/utility/file.h>
#include <glm-0.9.9/glm.hpp>

#include <iostream>

namespace Tutorial
{
    void Data::initialize()
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
        auto geometryShaderSource = Utility::File::getFileContents(geometryShaderPath);

        shaderPipelineDemoShader = Flare::GL::ShaderProgramBuilder()
            .addVertexShader(std::move(vertexShaderSource))
            .addFragmentShader(std::move(fragmentShaderSource))
            .addTessellationControlShader(std::move(tessellationControlShaderSource))
            .addTessellationEvaluationShader(std::move(tessellationEvaluationShaderSource))
            .addGeometryShader(std::move(geometryShaderSource))
            .build();

        shaderPipelineDemoShader->bind();

        //Describe the contents of the vertex buffers
        auto positionBufferLayout = Flare::GL::DataLayoutBuilder()
            .addVertexAttribute("position", 4, GL_FLOAT, GL_FALSE, 0)
            .setStride(sizeof(glm::vec4))
            .build();

        auto vertexOffsetBufferLayout = Flare::GL::DataLayoutBuilder()
            .addVertexAttribute("offset", 4, GL_FLOAT, GL_FALSE, 0)
            .setStride(sizeof(glm::vec4))
            .build();

        auto triangleColorBufferLayout = Flare::GL::DataLayoutBuilder()
            .addVertexAttribute("color", 4, GL_FLOAT, GL_FALSE, 0)
            .setStride(sizeof(glm::vec4))
            .build();

        //Create the actual buffers
        vertexPositionBuffer = std::make_unique<Flare::GL::Buffer>(positionBufferLayout);
        vertexOffsetBuffer = std::make_unique<Flare::GL::Buffer>(vertexOffsetBufferLayout);
        triangleColorBuffer = std::make_unique<Flare::GL::Buffer>(triangleColorBufferLayout);

        //Test out a VAO that sources inputs from multiple tightly-packed buffers
        auto VAOBuffers = std::vector<std::reference_wrapper<const Flare::GL::Buffer>>{
            *(vertexPositionBuffer.get()),
            *(vertexOffsetBuffer.get()),
            *(triangleColorBuffer.get())
        };

        //Link the shader attributes to the buffers
        shaderPipelineDemoShaderVAO = std::make_unique<Flare::GL::VertexArray>(
            *(shaderPipelineDemoShader.get()),
            VAOBuffers
        );

        glm::vec4 vertices[] = {
            glm::vec4(0.25, -0.25, 0.5, 1.0),
            glm::vec4(-0.25, -0.25, 0.5, 1.0),
            glm::vec4(0.25, 0.25, 0.5, 1.0)
        };

        glm::vec4 vertexColors[] = {
            glm::vec4(1.0f, 1.04, 0.25f, 1.0f),
            glm::vec4(1.0f, 1.04, 0.25f, 1.0f),
            glm::vec4(1.0f, 1.04, 0.25f, 1.0f)
        };

        vertexPositionBuffer->namedBufferStorage(sizeof(glm::vec4) * 3, &vertices[0], 0);
        triangleColorBuffer->namedBufferStorage((sizeof(glm::vec4)) * 3, &vertexColors[0], 0);
        vertexOffsetBuffer->namedBufferStorage((sizeof(glm::vec4)), 0, GL_DYNAMIC_STORAGE_BIT);

        //Number of vertices to use per patch for the tessellation control shader.
        //3 is the default, so this call is unnecessary but good for demonstration.
        glPatchParameteri(GL_PATCH_VERTICES, 3);

        //Wireframe rendering to view tessellated triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //Geometry shader breaks the triangles down and emits points. So increase
        //the point size for visibility for this demo
        glPointSize(5);
    }

    void Data::render(unsigned int deltaTime)
    {
        elapsedTime += deltaTime;

        auto scaledElapsedTime = elapsedTime * .001f;

        const GLfloat clearColor[] = {
            (float)sin(scaledElapsedTime) * 0.5f + 0.5f,
            (float)cos(scaledElapsedTime) * 0.5f + 0.5f,
            0.0f,
            1.0f
        };

        auto vertexOffset = glm::vec4{
            (float)sin(scaledElapsedTime) * 0.5f,
            (float)cos(scaledElapsedTime) * 0.6f,
            0.0f,
            0.0f
        };

        vertexOffsetBuffer->namedBufferSubData(0, sizeof(glm::vec4), &vertexOffset[0]);

        shaderPipelineDemoShaderVAO->bind();
        
        glClearBufferfv(GL_COLOR, 0, clearColor);

        //Have to draw GL_PATCHES instead of GL_TRIANGLES due to using the tessellation
        //shaders. GL_TRIANGLES renders nothing.
        glDrawArrays(GL_PATCHES, 0, 3);
        SDL_GL_SwapWindow(renderWindow->getRenderWindowHandle());
    }

    void Data::shutdown()
    {
        glDeleteVertexArrays(1, &VAO);
        renderWindow->freeResources();
    }
}
