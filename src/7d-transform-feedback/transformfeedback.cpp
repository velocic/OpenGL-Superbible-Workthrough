#include <7d-transform-feedback/transformfeedback.h>

#include <cmath>

#include <flare/rendersystem/factory.h>
#include <flare/gl/texturemanager.h>
#include <flare/gl/transformfeedbackbuffermanager.h>
#include <flare/gl/shaderprogram.h>
#include <flare/scenegraph/indirectrenderedscenegraph.h>

namespace Tutorial
{
    void TransformFeedback::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Transform Feedback: Particle Spring Simulation",
            1280,
            720
        );

        shaderManager = std::make_unique<Flare::RenderSystem::ShaderManager>();
        transformFeedbackBufferManager = std::make_unique<Flare::GL::TransformFeedbackBufferManager>();
        textureManager = std::make_unique<Flare::GL::TextureManager>();
        modelManager= std::make_unique<Flare::SceneGraph::ModelManager>(*textureManager.get());
        sceneGraph = std::make_unique<Flare::SceneGraph::IndirectRenderedSceneGraph>();

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

        auto transformFeedbackShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(transformFeedbackVertexShaderPath)
            .setFragmentShader(transformFeedbackFragmentShaderPath)
            .build();

        auto vertexBufferVAO = Flare::RenderSystem::createVertexArray(
            transformFeedbackShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout> {
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout},
                Flare::RenderSystem::VertexBufferVertexDataLayout{"mvpMatrix", mvpMatrixBufferLayout}
            }
        );

        shaderManager->insert(std::move(transformFeedbackShader), std::move(vertexBufferVAO), "transformFeedbackShader");

        setInitialVertexBufferState();
    }

    void TransformFeedback::render(unsigned int deltaTime)
    {
    }

    void TransformFeedback::shutdown()
    {
    }

    void TransformFeedback::initSamplers()
    {
    }

    void TransformFeedback::setInitialVertexBufferState()
    {
        auto combinePositionAndVelocityData = [](const auto &positions, const auto &velocities, size_t size){
            auto positionVelocityData = std::vector<float>();
            positionVelocityData.reserve(7 * size);

            for (size_t i = 0; i < size; ++i) {
                positionVelocityData.push_back(positions[i].x);
                positionVelocityData.push_back(positions[i].y);
                positionVelocityData.push_back(positions[i].z);
                positionVelocityData.push_back(positions[i].w);

                positionVelocityData.push_back(velocities[i].x);
                positionVelocityData.push_back(velocities[i].y);
                positionVelocityData.push_back(velocities[i].z);
            }

            return positionVelocityData;
        };

        constexpr auto totalVertices = verticesPerAxis * verticesPerAxis;

        auto initialPositions = std::vector<glm::vec4>(totalVertices);
        auto connectionVectors = std::vector<glm::ivec4>(totalVertices);
        auto initialVelocities = std::vector<glm::vec3>(totalVertices);

        auto parallelIndex = size_t{0};

        for (size_t yIndex = 0; yIndex < verticesPerAxis; ++yIndex) {
            auto normalizedY = yIndex / static_cast<float>(verticesPerAxis);

            for (size_t xIndex = 0; xIndex < verticesPerAxis; ++xIndex) {
                auto normalizedX = xIndex / static_cast<float>(verticesPerAxis);

                initialPositions[parallelIndex] = glm::vec4{
                    (normalizedX - 0.5f) * verticesPerAxis,
                    (normalizedY - 0.5f) * verticesPerAxis,
                    0.6f * sinf(normalizedX) * cosf(normalizedY),
                    1.0f
                };
                initialVelocities[parallelIndex] = glm::vec3(0.0f);
                connectionVectors[parallelIndex] = glm::ivec4(-1);

                if (yIndex != (verticesPerAxis - 1)) {
                    if (xIndex != 0) {
                        connectionVectors[parallelIndex].x = parallelIndex - 1;
                    }

                    if (yIndex != 0) {
                        connectionVectors[parallelIndex].y = parallelIndex - verticesPerAxis;
                    }

                    if (xIndex != verticesPerAxis - 1) {
                        connectionVectors[parallelIndex].z = parallelIndex + 1;
                    }

                    if (yIndex != verticesPerAxis - 1) {
                        connectionVectors[parallelIndex].w = parallelIndex + verticesPerAxis;
                    }
                }

                ++parallelIndex;
            }
        }

        auto initialPositionVelocityData = combinePositionAndVelocityData(initialPositions, initialVelocities, parallelIndex);

        auto positionVelocityBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("position_mass", 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .addAttribute("velocity", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 4)
            .build();

        auto connectionBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("connection", 4, Flare::RenderSystem::RS_INT, Flare::RenderSystem::RS_FALSE, 0)
            .build();

        auto positionVelocityBuffer = transformFeedbackBufferManager->create(
            shaderManager->get("transformFeedbackShader"),
            positionVelocityBufferLayout,
            initialPositionVelocityData.size() * (sizeof(glm::vec4) + sizeof(glm::vec3)),
            0,
            std::vector<std::string>{"tf_position_mass", "tf_velocity"},
            initialPositionVelocityData.data()
        );

        connectionBuffer = Flare::RenderSystem::createBuffer("connectionBuffer", connectionBufferLayout);
        connectionBuffer->allocateBufferStorage(connectionVectors.size() * sizeof(glm::ivec4), connectionVectors.data(), 0);

        auto positionVelocityTexture = textureManager->createTextureBuffer("positionVelocityTexture", Flare::RenderSystem::TextureManager::TextureInitParams{1, Flare::RenderSystem::RS_RGBA32F, false});
        positionVelocityTexture->attachBufferStorage(*positionVelocityBuffer);
    }

    void TransformFeedback::runParticleSimulationSteps()
    {
        transformFeedbackBufferManager->disableRasterization();

        for (size_t i = 0; i < particleSimStepsPerFrame; ++i) {
            if (i % 2 == 0) {
                //attach positionA, velocityA buffers to node
            } else {
                //attach positionB, velocityB buffers to node
            }

            transformFeedbackBufferManager->beginTransformFeedback(GL_POINTS);
            sceneGraph->render();
            transformFeedbackBufferManager->endTransformFeedback();
        }

        transformFeedbackBufferManager->enableRasterization();
    }
}
