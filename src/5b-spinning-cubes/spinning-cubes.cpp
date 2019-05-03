#include <5b-spinning-cubes/spinning-cubes.h>

#include <cmath>

#include <flare/utility/file.h>
#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>

namespace Tutorial
{
    void SpinningCubes::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Spinning 3D Cubes",
            1280,
            720
        );

        spinningCubeShader = Flare::GL::ShaderProgramBuilder()
            .addVertexShader(Utility::File::getFileContents(vertexShaderPath))
            .addFragmentShader(Utility::File::getFileContents(fragmentShaderPath))
            .build();

        spinningCubeShader->bind();

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

        auto cubeMeshBufferLayout = Flare::GL::VertexDataLayoutBuilder()
            .addAttribute("position", 3, GL_FLOAT, GL_FALSE, 0)
            .setStride(sizeof(GLfloat) * 3)
            .build();

        spinningCubeShader->addUniformAttribute("mv_matrix");
        spinningCubeShader->addUniformAttribute("proj_matrix");

        cubeMeshBuffer = std::make_unique<Flare::GL::Buffer>(cubeMeshBufferLayout);

        auto bufferRefsForCubeMeshVAO = std::vector<std::reference_wrapper<const Flare::GL::Buffer>>{
            *(cubeMeshBuffer.get())
        };

        cubeMeshVAO = std::make_unique<Flare::GL::VertexArray>(
            *(spinningCubeShader.get()),
            bufferRefsForCubeMeshVAO
        );

        cubeMeshBuffer->namedBufferStorage(cubeVertexPositions.max_size() * sizeof(GLfloat), cubeVertexPositions.data(), 0);
    }

    void SpinningCubes::render(unsigned int deltaTime)
    {
        elapsedTime += deltaTime;
        float f = static_cast<float>(elapsedTime) * PI * 0.1f;

        auto identityMatrix = glm::mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        auto modelViewMatrix = 
            glm::translate(
                identityMatrix,
                glm::vec3(0.0f, 0.0f, -4.0f)
            ) *
            glm::translate(
                identityMatrix,
                glm::vec3(
                    sinf(2.1f * f) * 0.5f,
                    cosf(1.7f * f) * 0.5f,
                    sinf(1.3f * f) * cosf(1.5f * f) * 2.0f
                )
            ) *
            glm::rotate(
                identityMatrix,
                static_cast<float>(elapsedTime) * 45.0f,
                glm::vec3(0.0f, 1.0f, 0.0f)
            ) *
            glm::rotate(
                identityMatrix,
                static_cast<float>(elapsedTime) * 81.0f,
                glm::vec3(1.0f, 0.0f, 0.0f)
            );

        auto perspectiveMatrix = glm::perspective(
            50.0f,
            renderWindow->getAspectRatio(),
            0.1f,
            1000.0f
        );

        cubeMeshVAO->bind();

        glUniformMatrix4fv(
            spinningCubeShader->getUniformAttribute("mv_matrix"),
            1,
            GL_FALSE,
            &modelViewMatrix[0][0]
        );

        glUniformMatrix4fv(
            spinningCubeShader->getUniformAttribute("proj_matrix"),
            1,
            GL_FALSE,
            &perspectiveMatrix[0][0]
        );

        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        SDL_GL_SwapWindow(renderWindow->getRenderWindowHandle());
    }

    void SpinningCubes::shutdown()
    {
        renderWindow->freeResources();
    }
}
