#include <5b-spinning-cubes/spinning-cubes.h>

#include <cmath>

#include <flare/gl/sampler.h>
#include <flare/gl/texture2d.h>
#include <flare/utility/file.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>
#include <lodepng/lodepng.h>

namespace Tutorial
{
    void SpinningCubes::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Spinning 3D Cubes",
            1280,
            720
        );

        auto demoTextureSampler = std::make_unique<Flare::GL::Sampler>("demoTexture");
        demoTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        demoTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        demoTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        demoTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        spinningCubeShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .addTextureUnit(demoTextureSampler.get())
            .build();

        textureManager->loadTexture2D(
            Flare::RenderSystem::TextureManager::PhongTextureFile{
                "../src/5b-spinning-cubes/textures/brick.png",
                "brick",
                Flare::RenderSystem::TextureManager::PhongMaterialTextureType::DIFFUSE,
                Flare::RenderSystem::TextureManager::SupportedFileType::PNG,
                GL_RGBA
            },
            Flare::RenderSystem::TextureManager::TextureInitParams{
                1,
                GL_RGBA8,
                false
            },
            [&](auto loadedTexture){
                spinningCubeShader->setTexture(demoTextureSampler->getName(), loadedTexture.diffuse[0]);
            }
        );

        spinningCubeShader->bind();

        cubeVertexPositions = std::array<GLfloat, 108>{
            -0.25f, 0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            0.25f, -0.25f, -0.25f,
            
            0.25f, -0.25f, -0.25f,
            0.25f, 0.25f, -0.25f,
            -0.25f, 0.25f, -0.25f,
            
            0.25f, -0.25f, -0.25f,
            0.25f, -0.25f, 0.25f,
            0.25f, 0.25f, -0.25f,
            
            0.25f, -0.25f, 0.25f,
            0.25f, 0.25f, 0.25f,
            0.25f, 0.25f, -0.25f,
            
            0.25f, -0.25f, 0.25f,
            -0.25f, -0.25f, 0.25f,
            0.25f, 0.25f, 0.25f,
            
            -0.25f, -0.25f, 0.25f,
            -0.25f, 0.25f, 0.25f,
            0.25f, 0.25f, 0.25f,
            
            -0.25f, -0.25f, 0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f, 0.25f, 0.25f,
            
            -0.25f, -0.25f, -0.25f,
            -0.25f, 0.25f, -0.25f,
            -0.25f, 0.25f, 0.25f,
            
            -0.25f, -0.25f, 0.25f,
            0.25f, -0.25f, 0.25f,
            0.25f, -0.25f, -0.25f,
            
            0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f, 0.25f,
            
            -0.25f, 0.25f, -0.25f,
            0.25f, 0.25f, -0.25f,
            0.25f, 0.25f, 0.25f,
            
            0.25f, 0.25f, 0.25f,
            -0.25f, 0.25f, 0.25f,
            -0.25f, 0.25f, -0.25f
        };

        auto cubeMeshBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("position", 3, GL_FLOAT, GL_FALSE, 0)
            .setStride(sizeof(GLfloat) * 3)
            .build();

        spinningCubeShader->addUniformAttribute("mv_matrix");
        spinningCubeShader->addUniformAttribute("proj_matrix");

        cubeMeshBuffer = std::make_unique<Flare::GL::Buffer>("cubeMeshBuffer", cubeMeshBufferLayout);

        auto bufferLayoutsForCubeMeshVAO = std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
            Flare::RenderSystem::VertexBufferVertexDataLayout{"cubeMeshBuffer", cubeMeshBufferLayout}
        };

        cubeMeshVAO = std::make_unique<Flare::GL::VertexArray>(
            spinningCubeShader.get(),
            bufferLayoutsForCubeMeshVAO
        );
        cubeMeshVAO->linkBuffers(std::vector<std::reference_wrapper<const Flare::RenderSystem::Buffer>>{*cubeMeshBuffer.get()});

        cubeMeshBuffer->allocateBufferStorage(cubeVertexPositions.max_size() * sizeof(GLfloat), cubeVertexPositions.data(), 0);
    }

    void SpinningCubes::render(unsigned int deltaTime)
    {
        elapsedTime += deltaTime;
        auto dampeningValue = 0.00005f;

        auto identityMatrix = glm::mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        auto perspectiveMatrix = glm::perspective(
            50.0f,
            renderWindow->getAspectRatio(),
            0.1f,
            1000.0f
        );

        cubeMeshVAO->bind();

        glUniformMatrix4fv(
            spinningCubeShader->getUniformAttribute("proj_matrix"),
            1,
            GL_FALSE,
            &perspectiveMatrix[0][0]
        );

        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);

        //Draw 24 cubes...
        for (int i = 0; i < 24; ++i) {
            float f = static_cast<float>(i) + static_cast<float>(elapsedTime) * 0.3f * dampeningValue;

            auto modelViewMatrix = 
                glm::translate(
                    identityMatrix,
                    glm::vec3(0.0f, 0.0f, -20.0f)
                ) *
                glm::rotate(
                    identityMatrix,
                    static_cast<float>(elapsedTime * dampeningValue) * 45.0f,
                    glm::vec3(0.0f, 1.0f, 0.0f)
                ) *
                glm::rotate(
                    identityMatrix,
                    static_cast<float>(elapsedTime * dampeningValue) * 21.0f,
                    glm::vec3(1.0f, 0.0f, 0.0f)
                ) * 
                glm::translate(
                    identityMatrix,
                    glm::vec3(
                        sinf(2.1f * f) * 2.0f,
                        cosf(1.7f * f) * 2.0f,
                        sinf(1.3f * f) * cosf(1.5f * f) * 2.0f
                    )
                );

            glUniformMatrix4fv(
                spinningCubeShader->getUniformAttribute("mv_matrix"),
                1,
                GL_FALSE,
                &modelViewMatrix[0][0]
            );

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        renderWindow->swapWindow();
    }

    void SpinningCubes::shutdown()
    {
        renderWindow->freeResources();
    }
}
