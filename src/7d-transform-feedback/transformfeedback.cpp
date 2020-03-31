#include <7d-transform-feedback/transformfeedback.h>

#include <cmath>

#include <flare/rendersystem/factory.h>
#include <flare/gl/texturemanager.h>
#include <flare/gl/transformfeedbackbuffermanager.h>
#include <flare/gl/shaderprogram.h>

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

        auto vertexBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("position_mass", 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .addAttribute("velocity", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec4))
            .addAttribute("connection", 4, Flare::RenderSystem::RS_INT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec4) + sizeof(glm::vec3))
            .setStride(2 * sizeof(glm::vec4) + sizeof(glm::vec3))
            .build();

        // auto mvpMatrixBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
        //     .addMatrixAttribute("mvpMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
        //     .setStride(sizeof(glm::mat4))
        //     .setDivisor(1)
        //     .build();

        auto transformFeedbackShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(transformFeedbackVertexShaderPath)
            .setFragmentShader(transformFeedbackFragmentShaderPath)
            .build();

        auto vertexBufferVAO = Flare::RenderSystem::createVertexArray(
            transformFeedbackShader.get(),
            std::vector<Flare::RenderSystem::VertexBufferVertexDataLayout> {
                Flare::RenderSystem::VertexBufferVertexDataLayout{"vertexBuffer", vertexBufferLayout}
            }
        );

        shaderManager->insert(std::move(transformFeedbackShader), std::move(vertexBufferVAO), "transformFeedbackShader");

        auto transformFeedbackVaryings = std::vector<std::string> {
            "tf_position_mass",
            "tf_velocity"
        };

        constexpr auto totalNumVertices = verticesPerAxis * verticesPerAxis;
        constexpr auto transformFeedbackBufferSize = (totalNumVertices * (sizeof(glm::vec4) * 2))
            + (totalNumVertices * sizeof(glm::vec3));

        transformFeedbackBufferManager->create(
            shaderManager->get("transformFeedbackShader"),
            vertexBufferLayout,
            transformFeedbackBufferSize,
            0,
            transformFeedbackVaryings
        );
    }

    void TransformFeedback::render(unsigned int deltaTime)
    {
    }

    void TransformFeedback::shutdown()
    {
    }

    void TransformFeedback::setInitialVertexBufferState()
    {
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

        //TODO: buffer data to textures
    }
}
