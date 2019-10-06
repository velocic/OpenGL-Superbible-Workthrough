#include <7a-instancing/instancing.h>

#include <flare/gl/texturemanager.h>
#include <flare/gl/shaderprogram.h>
#include <flare/rendersystem/factory.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>

namespace Tutorial
{
    void Instancing::initializeMVPMatrixBuffer(const Flare::RenderSystem::VertexDataLayout &bufferLayout)
    {
        mvpMatrixBuffer = Flare::RenderSystem::createBuffer("mvpMatrix", bufferLayout);
        mvpMatrixBuffer->allocateBufferStorage(sizeof(glm::mat4) * 1000, nullptr, GL_MAP_WRITE_BIT);
        auto bufferMapping = mvpMatrixBuffer->mapRange(0, sizeof(glm::mat4) * 1000);

        auto writableBuffer = reinterpret_cast<glm::mat4*>(bufferMapping->get());

        auto identityMatrix = glm::mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 0
        };

        for (size_t x = 0; x < 10; ++x) {
            for (size_t y = 0; y < 10; ++y) {
                for (size_t z = 0; z < 10; z++) {
                    writableBuffer[x * y * z] = glm::translate(identityMatrix, glm::vec3(x, y, -z - 5));
                }
            }
        }

        mvpMatrixBuffer->unmap();
    }

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

        auto mvpMatrixBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addMatrixAttribute("mvpMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0, 1)
            .setStride(sizeof(glm::mat4))
            .build();

        auto instanceShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .build();

        initializeMVPMatrixBuffer(mvpMatrixBufferLayout);

        auto instanceShaderVAO = Flare::RenderSystem::createVertexArray(
            instanceShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout},
                Flare::RenderSystem::VertexBufferVertexDataLayout{"mvpMatrix", mvpMatrixBufferLayout}
            }
        );

        shaderManager->insert(
            std::move(instanceShader),
            std::move(instanceShaderVAO),
            "instanceShader"
        );
        shaderManager->get("instanceShader").vertexArray->linkBuffers(
            std::vector<std::reference_wrapper<const Flare::RenderSystem::Buffer>>{*mvpMatrixBuffer.get()}
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
