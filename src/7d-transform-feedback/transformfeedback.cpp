#include <7d-transform-feedback/transformfeedback.h>

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

        auto vertexBufferLayout = Flare::RenderSystem::VertexDataLayoutBuilder()
            .addAttribute("position_mass", 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
            .addAttribute("velocity", 3, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec4))
            .addAttribute("connection", 4, Flare::RenderSystem::RS_INT, Flare::RenderSystem::RS_FALSE, sizeof(glm::vec4) + sizeof(glm::vec3))
            .setStride(2 * sizeof(glm::vec4) + sizeof(glm::vec3))
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
    }

    void TransformFeedback::render(unsigned int deltaTime)
    {
    }

    void TransformFeedback::shutdown()
    {
    }
}
