#ifndef TUTORIAL_TRANSFORMFEEDBACK_H
#define TUTORIAL_TRANSFORMFEEDBACK_H

#include <memory>

#include <flare/application.h>
#include <flare/rendersystem/shadermanager.h>
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

            const std::string transformFeedbackVertexShaderPath{"../src/7d-transform-feedback/shaders/transformfeedback-vertex.glsl"};
            const std::string transformFeedbackFragmentShaderPath{"../src/7d-transform-feedback/shaders/transformfeedback-fragment.glsl"};
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
}

#endif
