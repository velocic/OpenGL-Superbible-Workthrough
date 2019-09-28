#include <6a-model-file-loading/model-file-loading.h>

#include <iostream>

#include <flare/gl/texturemanager.h>
#include <flare/gl/sampler.h>
#include <flare/gl/shaderprogram.h>
#include <flare/scenegraph/modelmanager.h>
#include <flare/rendersystem/factory.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>

namespace Tutorial
{
    void ModelFileLoading::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Loading a 3D model file",
            1280,
            720
        );

        textureManager = std::make_unique<Flare::GL::TextureManager>();
        modelManager = std::make_unique<Flare::SceneGraph::ModelManager>(*textureManager.get());
        shaderManager = std::make_unique<Flare::RenderSystem::ShaderManager>();

        diffuseTextureSampler = std::make_unique<Flare::GL::Sampler>("diffuse_texture");
        specularTextureSampler = std::make_unique<Flare::GL::Sampler>("specular_texture");

        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        specularTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        auto vertexBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("position", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .addAttribute("normal", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec3))
            .addAttribute("uvCoord", 2, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec3) * 2)
            .setStride(sizeof(Flare::DataTypes::Vertex))
            .build();

        auto basicUntexturedUnlitMeshDisplayShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .build();
        basicUntexturedUnlitMeshDisplayShader->addUniformAttribute("mv_matrix");
        basicUntexturedUnlitMeshDisplayShader->addUniformAttribute("proj_matrix");

        auto untexturedUnlitMeshDisplayVAO = Flare::RenderSystem::createVertexArray(
            basicUntexturedUnlitMeshDisplayShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout}
            }
        );

        shaderManager->insert(
            std::move(basicUntexturedUnlitMeshDisplayShader),
            std::move(untexturedUnlitMeshDisplayVAO),
            "untexturedUnlitMeshDisplay"
        );

        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{"stanford-bunny", "../src/6a-model-file-loading/models/stanford-bunny/stanford-bunny.obj"},
            [](auto){
                std::cout << "stanford bunny model loaded" << std::endl;
            }
        );
        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{"lantern", "../src/6a-model-file-loading/models/Lantern/Lantern.gltf"},
            [](auto) {
                std::cout << "lantern model loaded" << std::endl;
            }
        );
    }

    void ModelFileLoading::render(unsigned int deltaTime)
    {
        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);

        auto untexturedUnlitMeshDisplayShaderData = shaderManager->get("untexturedUnlitMeshDisplay");

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

        elapsedTime += deltaTime;
        auto dampeningValue = 0.00005f;
        float f = static_cast<float>(elapsedTime) * 0.3f * dampeningValue;

        auto modelViewMatrix =
            glm::translate(
                identityMatrix,
                glm::vec3(0.0f, 0.0f, -5.0f)
            ) *
            glm::rotate(
                identityMatrix,
                static_cast<float>(elapsedTime * dampeningValue) * 20.0f,
                glm::vec3(0.0f, 1.0f, 0.0f)
            ) *
            glm::rotate(
                identityMatrix,
                static_cast<float>(elapsedTime * dampeningValue) * 10.0f,
                glm::vec3(1.0f, 0.0f, 0.0f)
            ) *
            glm::translate(
                identityMatrix,
                glm::vec3(
                    sinf(2.1f * f) * 0.5f,
                    cosf(1.7f * f) * 0.5f,
                    sinf(1.3f * f) * cosf(1.5f * f) * 0.5f
                )
            );

        untexturedUnlitMeshDisplayShaderData.shader->bind();
        untexturedUnlitMeshDisplayShaderData.vertexArray->bind();
        untexturedUnlitMeshDisplayShaderData.shader->setUniformMatrix<4, 4, 1>(
            untexturedUnlitMeshDisplayShaderData.shader->getUniformAttribute("proj_matrix"),
            GL_FALSE,
            &perspectiveMatrix[0][0]
        );
        untexturedUnlitMeshDisplayShaderData.shader->setUniformMatrix<4, 4, 1>(
            untexturedUnlitMeshDisplayShaderData.shader->getUniformAttribute("mv_matrix"),
            GL_FALSE,
            &modelViewMatrix[0][0]
        );

        auto bunnyModel = modelManager->get("stanford-bunny");
        auto lanternModel = modelManager->get("lantern");
        bunnyModel->render(untexturedUnlitMeshDisplayShaderData, 1);

        auto lanternMVMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -100.0f));
        untexturedUnlitMeshDisplayShaderData.shader->setUniformMatrix<4, 4, 1>(
            untexturedUnlitMeshDisplayShaderData.shader->getUniformAttribute("mv_matrix"),
            GL_FALSE,
            &lanternMVMatrix[0][0]
        );
        lanternModel->render(untexturedUnlitMeshDisplayShaderData, 1);
        renderWindow->swapWindow();
    }

    void ModelFileLoading::shutdown()
    {
    }
}
