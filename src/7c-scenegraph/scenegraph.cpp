#include <7c-scenegraph/scenegraph.h>

#include <cmath>

#include <flare/gl/texturemanager.h>
#include <flare/gl/sampler.h>
#include <flare/gl/shaderprogram.h>
#include <flare/rendersystem/factory.h>

namespace Tutorial
{
    void SceneGraph::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Rendering a scene with a SceneGraph",
            1280,
            720
        );

        textureManager = std::make_unique<Flare::GL::TextureManager>();
        modelManager = std::make_unique<Flare::SceneGraph::ModelManager>(*textureManager.get());
        shaderManager = std::make_unique<Flare::RenderSystem::ShaderManager>();
        samplerManager = std::make_unique<Flare::RenderSystem::SamplerManager>();
        sceneGraph = std::make_unique<Flare::SceneGraph::SceneGraph>();
        initSamplers();

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

        auto bunnyShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(bunnyVertexShaderPath)
            .setFragmentShader(bunnyFragmentShaderPath)
            .build();

        auto lanternShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(lanternVertexShaderPath)
            .setFragmentShader(lanternFragmentShaderPath)
            .addTextureUnit(samplerManager->get("textureDiffuse0"))
            .addTextureUnit(samplerManager->get("textureDiffuse1"))
            .addTextureUnit(samplerManager->get("textureSpecular0"))
            .addTextureUnit(samplerManager->get("textureNormal0"))
            .build();

        bunnyShader->addUniformAttribute("vpMatrix");
        lanternShader->addUniformAttribute("vpMatrix");

        auto bunnyVAO = Flare::RenderSystem::createVertexArray(
            bunnyShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout},
                Flare::RenderSystem::VertexBufferVertexDataLayout{"mvpMatrix", mvpMatrixBufferLayout}
            }
        );
        auto lanternVAO = Flare::RenderSystem::createVertexArray(
            lanternShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout},
                Flare::RenderSystem::VertexBufferVertexDataLayout{"mvpMatrix", mvpMatrixBufferLayout}
            }
        );

        shaderManager->insert(std::move(bunnyShader), std::move(bunnyVAO), "bunnyShader");
        shaderManager->insert(std::move(lanternShader), std::move(lanternVAO), "lanternShader");

        modelManager->load(
            Flare::SceneGraph::ModelManager::ModelFile{"bunny", "../src/common-resources/models/stanford-bunny/stanford-bunny.obj"},
            [](auto){}
        );
        modelManager->loadWithPackedSubMeshes(
            Flare::SceneGraph::ModelManager::ModelFile{"lantern", "../src/common-resources/models/Lantern/Lantern.gltf"},
            [](auto){}
        );

        initScene();
        initNodes();
    }

    void SceneGraph::render(unsigned int deltaTime)
    {
        elapsedTime += deltaTime;
        bunnyNodes[0]->rotateNode(elapsedTime * (0.15/60.0f) * Flare::Math::degreesToRadians(36/360.0f), glm::vec3(0, 0.5, 0.5));
        lanternNodes[0]->setNodePosition(glm::vec3(std::sin(elapsedTime * (0.001)) * 15, 0, -200));
        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);
        sceneGraph->render();
        renderWindow->swapWindow();
    }

    void SceneGraph::shutdown()
    {
    }

    void SceneGraph::initNodes()
    {
        auto bunnyShaderData = shaderManager->get("bunnyShader");
        auto bunnyModel = modelManager->get("bunny");
        auto lanternShaderData = shaderManager->get("lanternShader");
        auto lanternModel = modelManager->get("lantern");

        auto testBunnyNode = sceneGraph->createNode(sceneGraph->getRootNode(), bunnyModel);
        auto bunnyCubeCenterToOriginVector = glm::vec3(9 * .25, 9 * .25, 9 * .25) * .5f;
        for (size_t x = 0; x < 10; ++x) {
            for (size_t y = 0; y < 10; ++y) {
                for (size_t z = 0; z < 10; z++) {
                    auto instanceId = testBunnyNode->addInstance();
                    testBunnyNode->translateInstance(instanceId, glm::vec3(x * .25, y * .25, z * .25) - bunnyCubeCenterToOriginVector);
                }
            }
        }
        testBunnyNode->translateNode(glm::vec3(-0.5, -0.5, -25));
        testBunnyNode->scaleNode(glm::vec3(2, 2, 2));
        testBunnyNode->setShaderData(bunnyShaderData);
        bunnyNodes.push_back(testBunnyNode);

        auto testLanternNode = sceneGraph->createNode(sceneGraph->getRootNode(), lanternModel);
        testLanternNode->addInstance();
        testLanternNode->setShaderData(lanternShaderData);
        // testLanternNode->scaleNode(glm::vec3(0.5, 0.5, 0.5));
        testLanternNode->addChildNode(testBunnyNode);
        testLanternNode->translateNode(glm::vec3(0, 0, -200));
        lanternNodes.push_back(testLanternNode);
    }

    void SceneGraph::initScene()
    {
        auto projectionMatrix = glm::perspective(
            50.0f,
            renderWindow->getAspectRatio(),
            0.1f,
            1000.0f
        );
        auto viewMatrix = Flare::Math::identityMatrix;
        auto vpMatrix = viewMatrix * projectionMatrix;

        auto bunnyShaderData = shaderManager->get("bunnyShader");
        auto lanternShaderData = shaderManager->get("lanternShader");

        bunnyShaderData.shader->bind();
        bunnyShaderData.vertexArray->bind();
        bunnyShaderData.shader->setUniformMatrix<4, 4, 1>(
            bunnyShaderData.shader->getUniformAttribute("vpMatrix"),
            GL_FALSE,
            &vpMatrix[0][0]
        );

        lanternShaderData.shader->bind();
        lanternShaderData.vertexArray->bind();
        lanternShaderData.shader->setUniformMatrix<4, 4, 1>(
            lanternShaderData.shader->getUniformAttribute("vpMatrix"),
            GL_FALSE,
            &vpMatrix[0][0]
        );
    }

    void SceneGraph::initSamplers()
    {
        auto diffuse0TextureSampler = std::make_unique<Flare::GL::Sampler>("textureDiffuse0");
        auto diffuse1TextureSampler = std::make_unique<Flare::GL::Sampler>("textureDiffuse1");
        auto specularTextureSampler = std::make_unique<Flare::GL::Sampler>("textureSpecular0");
        auto normalTextureSampler = std::make_unique<Flare::GL::Sampler>("textureNormal0");

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

        samplerManager->insert(std::move(diffuse0TextureSampler), "textureDiffuse0");
        samplerManager->insert(std::move(diffuse1TextureSampler), "textureDiffuse1");
        samplerManager->insert(std::move(specularTextureSampler), "textureSpecular0");
        samplerManager->insert(std::move(normalTextureSampler), "textureNormal0");
    }
}
