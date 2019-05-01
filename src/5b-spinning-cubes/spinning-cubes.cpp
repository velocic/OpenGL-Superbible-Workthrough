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

        cubeMeshBuffer = std::make_unique<Flare::GL::Buffer>(cubeMeshBufferLayout);
        cubeMeshBuffer->namedBufferStorage(cubeVertexPositions.max_size() * sizeof(GLfloat), cubeVertexPositions.data(), GL_STATIC_DRAW);

        auto bufferRefsForCubeMeshVAO = std::vector<std::reference_wrapper<const Flare::GL::Buffer>>{
            *(cubeMeshBuffer.get())
        };

        cubeMeshVAO = std::make_unique<Flare::GL::VertexArray>(
            *(spinningCubeShader.get()),
            bufferRefsForCubeMeshVAO
        );
    }

    void SpinningCubes::render(unsigned int deltaTime)
    {
    }

    void SpinningCubes::shutdown()
    {
    }
}
