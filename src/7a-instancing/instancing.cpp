#include <7a-instancing/instancing.h>

#include <flare/gl/texturemanager.h>
#include <flare/gl/shaderprogram.h>
#include <flare/rendersystem/factory.h>

namespace Tutorial
{
    void Instancing::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Instanced models",
            1280,
            720
        );

        textureManager = std::make_unique<Flare::GL::TextureManager>();
        modelManager = std::make_unique<Flare::SceneGraph::ModelManager>(*textureManager.get());
        shaderManager = std::make_unique<Flare::RenderSystem::ShaderManager>();

        auto vertexBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("position", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .setStride(sizeof(Flare::DataTypes::Vertex))
            .build();

        // auto mvpMatrixBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
        //     .addAttribute("mvpMatrix", 1, )

        auto instanceShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .build();
        instanceShader->addUniformAttribute("mvpMatrix");

        auto instanceShaderVAO = Flare::RenderSystem::createVertexArray(
            instanceShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout}
            }
        );

        shaderManager->insert(
            std::move(instanceShader),
            std::move(instanceShaderVAO),
            "instanceShader"
        );

        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{"stanford-bunny", "../src/common-resources/models/stanford-bunny/stanford-bunny.obj"},
            [](auto){}
        );
    }

    void Instancing::render(unsigned int deltaTime)
    {
    }

    void Instancing::shutdown()
    {
    }
}
