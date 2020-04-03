#include <7b-packed-mesh-rendering/packed-mesh-rendering.h>

#include <flare/gl/texturemanager.h>
#include <flare/gl/sampler.h>
#include <flare/gl/shaderprogram.h>
#include <flare/rendersystem/factory.h>
#include <flare/utility/math.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>

namespace Tutorial
{
    void PackedMeshRendering::initializeMVPMatrixBuffer(const Flare::RenderSystem::VertexDataLayout &bufferLayout)
    {
        auto projectionMatrix = glm::perspective(
            50.0f,
            renderWindow->getAspectRatio(),
            0.1f,
            1000.0f
        );
        auto mvpMatrix = projectionMatrix * glm::translate(Flare::Math::identityMatrix, glm::vec3(0.0f, 0.0f, -100.0f));
        mvpMatrixBuffer = Flare::RenderSystem::createBuffer("mvpMatrix", bufferLayout);
        mvpMatrixBuffer->allocateBufferStorage(sizeof(glm::mat4), &mvpMatrix[0][0], 0);
    }

    void PackedMeshRendering::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Rendering multiple meshes packed into a single buffer",
            1280,
            720
        );

        textureManager = std::make_unique<Flare::GL::TextureManager>();
        modelManager = std::make_unique<Flare::SceneGraph::ModelManager>(*textureManager.get());
        shaderManager = std::make_unique<Flare::RenderSystem::ShaderManager>();

        diffuse0TextureSampler = std::make_unique<Flare::GL::Sampler>("textureDiffuse0");
        diffuse1TextureSampler = std::make_unique<Flare::GL::Sampler>("textureDiffuse1");
        specularTextureSampler = std::make_unique<Flare::GL::Sampler>("textureSpecular0");
        normalTextureSampler = std::make_unique<Flare::GL::Sampler>("textureNormal0");

        diffuse0TextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        diffuse0TextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        diffuse0TextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        diffuse0TextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        diffuse1TextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        diffuse1TextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        diffuse1TextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        diffuse1TextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        specularTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        normalTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        normalTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        normalTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        normalTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        auto vertexBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("position", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .addAttribute("normal", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec3))
            .addAttribute("uvCoord", 2, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec3) * 2)
            .setStride(sizeof(Flare::DataTypes::Vertex))
            .build();
        auto mvpMatrixBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addMatrixAttribute("mvpMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .setStride(sizeof(glm::mat4))
            .setDivisor(1)
            .build();

        auto shader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .addTextureUnit(diffuse0TextureSampler.get())
            .addTextureUnit(diffuse1TextureSampler.get())
            .addTextureUnit(specularTextureSampler.get())
            .addTextureUnit(normalTextureSampler.get())
            .build();

        initializeMVPMatrixBuffer(mvpMatrixBufferLayout);

        auto VAO = Flare::RenderSystem::createVertexArray(
            shader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout},
                Flare::RenderSystem::VertexBufferVertexDataLayout{"mvpMatrix", mvpMatrixBufferLayout}
            }
        );

        shaderManager->insert(
            std::move(shader),
            std::move(VAO),
            "shader"
        );

        modelManager->loadWithPackedSubMeshes(
            Flare::SceneGraph::ModelManager::ModelFile{"lantern", "../src/common-resources/models/Lantern/Lantern.gltf"},
            [](auto){}
        );
    }

    void PackedMeshRendering::render(unsigned int deltaTime)
    {
        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);
        auto shaderData = shaderManager->get("shader");
        auto lanternModel = modelManager->get("lantern");
        lanternModel->render(shaderData, *mvpMatrixBuffer.get(), additionalBuffersForModelShader, 1);
        renderWindow->swapWindow();
    }

    void PackedMeshRendering::shutdown()
    {
    }
}
