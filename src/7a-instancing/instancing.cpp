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
            0, 0, 0, 1
        };

        auto projectionMatrix = glm::perspective(
            50.0f,
            renderWindow->getAspectRatio(),
            0.1f,
            1000.0f
        );

        for (int x = 0; x < 10; ++x) {
            for (int y = 0; y < 10; ++y) {
                for (int z = 0; z < 10; z++) {
                    auto xCoord = x % 2 == 0 ? x : -x;
                    auto yCoord = y % 2 == 0 ? y : -y;
                    auto zCoord = z % 2 == 0 ? z : -z;

                    writableBuffer[x * y * z] = projectionMatrix * glm::translate(
                        identityMatrix,
                        glm::vec3(xCoord * .05, yCoord * .05, (zCoord * .05) - 15)
                    );
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
            .addMatrixAttribute("mvpMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .setStride(sizeof(glm::mat4))
            .setDivisor(1)
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

        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{"stanford-bunny", "../src/common-resources/models/stanford-bunny/stanford-bunny.obj"},
            [](auto){}
        );
    }

    void Instancing::render(unsigned int deltaTime)
    {
        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);
        auto instanceShaderData = shaderManager->get("instanceShader");
        auto bunnyModel = modelManager->get("stanford-bunny");
        bunnyModel->render(instanceShaderData, *mvpMatrixBuffer.get(), 1000);
        renderWindow->swapWindow();
    }

    void Instancing::shutdown()
    {
    }
}
