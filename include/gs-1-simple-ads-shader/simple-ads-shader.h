#ifndef TUTORIAL_SIMPLE_ADS_SHADER_H
#define TUTORIAL_SIMPLE_ADS_SHADER_H

#include <memory>

#include <flare/application.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/modelmanager.h>
#include <flare/scenegraph/scenegraph.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class SimpleADSShader : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow;
            std::unique_ptr<Flare::SceneGraph::ModelManager> modelManager;
            std::unique_ptr<Flare::RenderSystem::ShaderManager> shaderManager;
            std::unique_ptr<Flare::RenderSystem::TextureManager> textureManager;
            std::unique_ptr<Flare::SceneGraph::SceneGraph> sceneGraph;
            const std::string vertexShaderPath{"../src/gs-1-simple-ads-shader/shaders/basic-ads-vertex.glsl"};
            const std::string fragmentShaderPath{"../src/gs-1-simple-ads-shader/shaders/basic-ads-fragment.glsl"};
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
}

#endif
