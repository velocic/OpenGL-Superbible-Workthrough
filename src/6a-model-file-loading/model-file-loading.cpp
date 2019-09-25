#include <6a-model-file-loading/model-file-loading.h>

#include <iostream>

#include <flare/gl/texturemanager.h>
#include <flare/gl/sampler.h>
#include <flare/gl/shaderprogram.h>
#include <flare/scenegraph/modelmanager.h>

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

        basicUntexturedUnlitMeshDisplayShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .build();
        basicUntexturedUnlitMeshDisplayShader->addUniformAttribute("mv_matrix");
        basicUntexturedUnlitMeshDisplayShader->addUniformAttribute("proj_matrix");

        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{"stanford-bunny", "../src/6a-model-file-loading/models/stanford-bunny/stanford-bunny.obj"},
            [&](auto model){
                std::cout << "stanford bunny model loaded" << std::endl;
            }
        );
    }

    void ModelFileLoading::render(unsigned int deltaTime)
    {
        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);

        auto bunnyModel = modelManager->get("stanford-bunny");
        bunnyModel->render(basicUntexturedUnlitMeshDisplayShader.get(), 1);

        renderWindow->swapWindow();
    }

    void ModelFileLoading::shutdown()
    {
    }
}
