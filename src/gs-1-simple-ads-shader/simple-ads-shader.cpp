#include <gs-1-simple-ads-shader/simple-ads-shader.h>

#include <flare/gl/texturemanager.h>
#include <flare/gl/shaderprogram.h>
#include <flare/rendersystem/factory.h>

namespace Tutorial
{
    void SimpleADSShader::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "A Simple Vertex Lighting Shader With Stubbed Inputs",
            1280,
            720
        );

        textureManager = std::make_unique<Flare::GL::TextureManager>();
        modelManager = std::make_unique<Flare::SceneGraph::ModelManager>(*textureManager.get());
        shaderManager = std::make_unique<Flare::RenderSystem::ShaderManager>();
        sceneGraph = std::make_unique<Flare::SceneGraph::SceneGraph>();

        auto mvpMatrixBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addMatrixAttribute("modelMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .setStride(sizeof(glm::mat4))
            .setDivisor(1)
            .build();

        auto basicVertexLightingShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .build();
        basicVertexLightingShader->addUniformAttribute("projectionMatrix");

        auto basicVertexLightingVAO = Flare::RenderSystem::createVertexArray(
            basicVertexLightingShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"mvpMatrix", mvpMatrixBufferLayout}
            }
        );

        shaderManager->insert(
            std::move(basicVertexLightingShader),
            std::move(basicVertexLightingVAO),
            "basicVertexLightingShader"
        );

        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{
                "plane",
                "../src/common-resources/models/plane/plane.glb"
            },
            [](auto){}
        );
        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{
                "sphere",
                "../src/common-resources/models/sphere/sphere.glb"
            },
            [](auto){}
        );
    }

    void SimpleADSShader::render(unsigned int deltaTime)
    {
    }

    void SimpleADSShader::shutdown()
    {
    }

}
