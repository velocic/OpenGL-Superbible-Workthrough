#ifndef TUTORIAL_TRANSFORMFEEDBACK_H
#define TUTORIAL_TRANSFORMFEEDBACK_H

#include <memory>

#include <flare/application.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/transformfeedbackbuffermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/modelmanager.h>
#include <flare/scenegraph/scenegraph.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class TransformFeedback : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow;
            std::unique_ptr<Flare::RenderSystem::ShaderManager> shaderManager;
            std::unique_ptr<Flare::RenderSystem::TransformFeedbackBufferManager> transformFeedbackBufferManager;
            std::unique_ptr<Flare::RenderSystem::TextureManager> textureManager;
            std::unique_ptr<Flare::SceneGraph::ModelManager> modelManager;
            std::unique_ptr<Flare::SceneGraph::SceneGraph> sceneGraph;

            std::unique_ptr<Flare::RenderSystem::Buffer> positionBufferA;
            std::unique_ptr<Flare::RenderSystem::Buffer> positionBufferB;
            std::unique_ptr<Flare::RenderSystem::Buffer> velocityBufferA;
            std::unique_ptr<Flare::RenderSystem::Buffer> velocityBufferB;
            std::unique_ptr<Flare::RenderSystem::Buffer> connectionBuffer;

            const std::string transformFeedbackVertexShaderPath{"../src/7d-transform-feedback/shaders/transformfeedback-vertex.glsl"};
            const std::string transformFeedbackFragmentShaderPath{"../src/7d-transform-feedback/shaders/transformfeedback-fragment.glsl"};
            static constexpr auto verticesPerAxis = 50;
            static constexpr auto particleSimStepsPerFrame = 16;

            void initSamplers();
            void setInitialVertexBufferState();
            void runParticleSimulationSteps();
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
}

#endif
