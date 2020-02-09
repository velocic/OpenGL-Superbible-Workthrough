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

        loadShader("basicVertexLightingShaderA");
        loadShader("basicVertexLightingShaderB");

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
                "../src/common-resources/models/sphere/sphere-high-poly.glb"
            },
            [](auto){}
        );

        initScene();
        initNodes();
    }

    void SimpleADSShader::render(unsigned int deltaTime)
    {
        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);
        glClear(GL_DEPTH_BUFFER_BIT);

        sceneGraph->renderIndirect();

        renderWindow->swapWindow();
    }

    void SimpleADSShader::shutdown()
    {
    }

    void SimpleADSShader::loadShader(const std::string &shaderName)
    {
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

        auto basicVertexLightingShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .build();
        basicVertexLightingShader->addUniformAttribute("projectionMatrix");
        basicVertexLightingShader->addUniformAttribute("color");

        auto basicVertexLightingVAO = Flare::RenderSystem::createVertexArray(
            basicVertexLightingShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout>{
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout},
                Flare::RenderSystem::VertexBufferVertexDataLayout{"mvpMatrix", mvpMatrixBufferLayout}
            }
        );

        shaderManager->insert(
            std::move(basicVertexLightingShader),
            std::move(basicVertexLightingVAO),
            shaderName
        );
    }

    void SimpleADSShader::initScene()
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        auto projectionMatrix = glm::perspective(
            50.0f,
            renderWindow->getAspectRatio(),
            0.1f,
            1000.0f
        );

        auto basicVertexLightingShaderDataA = shaderManager->get("basicVertexLightingShaderA");
        basicVertexLightingShaderDataA.shader->bind();
        basicVertexLightingShaderDataA.vertexArray->bind();
        basicVertexLightingShaderDataA.shader->setUniformMatrix<4, 4, 1>(
            basicVertexLightingShaderDataA.shader->getUniformAttribute("projectionMatrix"),
            GL_FALSE,
            &projectionMatrix[0][0]
        );
        basicVertexLightingShaderDataA.shader->setUniform(
            basicVertexLightingShaderDataA.shader->getUniformAttribute("color"),
            orange.r, orange.g, orange.b, orange.a
        );

        auto basicVertexLightingShaderDataB = shaderManager->get("basicVertexLightingShaderB");
        basicVertexLightingShaderDataB.shader->bind();
        basicVertexLightingShaderDataB.vertexArray->bind();
        basicVertexLightingShaderDataB.shader->setUniformMatrix<4, 4, 1>(
            basicVertexLightingShaderDataB.shader->getUniformAttribute("projectionMatrix"),
            GL_FALSE,
            &projectionMatrix[0][0]
        );
        basicVertexLightingShaderDataB.shader->setUniform(
            basicVertexLightingShaderDataB.shader->getUniformAttribute("color"),
            blue.r, blue.g, blue.b, blue.a
        );
    }

    void SimpleADSShader::initNodes()
    {
        auto basicVertexLightingShaderDataA = shaderManager->get("basicVertexLightingShaderA");
        auto basicVertexLightingShaderDataB = shaderManager->get("basicVertexLightingShaderB");
        auto planeModel = modelManager->get("plane");
        auto sphereModel = modelManager->get("sphere");

        auto planeNode = sceneGraph->getRootNode()->createChildNode();
        planeNode->addInstance();
        planeNode->setShaderData(basicVertexLightingShaderDataA);
        planeNode->setModel(planeModel);
        planeNode->rotateNode(90.0f, glm::vec3(1, 0, 0));
        planeNode->setNodeScale(glm::vec3(7.0f, 1.0f, 3.5f));
        planeNode->setNodePosition(glm::vec3(0, 0, -40));

        auto sphereNode = sceneGraph->getRootNode()->createChildNode();
        sphereNode->addInstance();
        sphereNode->setShaderData(basicVertexLightingShaderDataB);
        sphereNode->setModel(sphereModel);
        sphereNode->setNodePosition(glm::vec3(0, 0, -20));
    }
}
