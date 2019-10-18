#ifndef TUTORIAL_SCENEGRAPH_H
#define TUTORIAL_SCENEGRAPH_H

#include <memory>

#include <flare/application.h>
#include <flare/rendersystem/samplermanager.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/modelmanager.h>
#include <flare/scenegraph/scenegraph.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class SceneGraph : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow;
            std::unique_ptr<Flare::RenderSystem::SamplerManager> samplerManager;
            std::unique_ptr<Flare::RenderSystem::ShaderManager> shaderManager;
            std::unique_ptr<Flare::RenderSystem::TextureManager> textureManager;
            std::unique_ptr<Flare::SceneGraph::ModelManager> modelManager;
            std::unique_ptr<Flare::SceneGraph::SceneGraph> sceneGraph;
            const std::string bunnyVertexShaderPath{"../src/7c-scenegraph/shaders/bunny-vertex.glsl"};
            const std::string bunnyFragmentShaderPath{"../src/7c-scenegraph/shaders/bunny-fragment.glsl"};
            const std::string lanternVertexShaderPath{"../src/7c-scenegraph/shaders/lantern-vertex.glsl"};
            const std::string lanternFragmentShaderPath{"../src/7c-scenegraph/shaders/lantern-fragment.glsl"};
            unsigned long long elapsedTime = 0;
            size_t instanceIdToRemove = 1000;

            std::vector<Flare::SceneGraph::Node *> bunnyNodes;
            std::vector<Flare::SceneGraph::Node *> lanternNodes;

            void initNodes();
            void initScene();
            void initSamplers();
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
}

#endif
