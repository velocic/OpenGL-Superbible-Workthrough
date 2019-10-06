#ifndef TUTORIAL_INSTANCING_H
#define TUTORIAL_INSTANCING_H

#include <memory>
#include <string>

#include <flare/application.h>
#include <flare/ui/renderwindow.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/modelmanager.h>

namespace Tutorial
{
    class Instancing : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::RenderSystem::ShaderManager> shaderManager = nullptr;
            std::unique_ptr<Flare::RenderSystem::TextureManager> textureManager = nullptr;
            std::unique_ptr<Flare::SceneGraph::ModelManager> modelManager = nullptr;
            std::unique_ptr<Flare::RenderSystem::Buffer> mvpMatrixBuffer = nullptr;
            const std::string vertexShaderPath{"../src/7a-instancing/shaders/vertex.glsl"};
            const std::string fragmentShaderPath{"../src/7a-instancing/shaders/fragment.glsl"};

            void initializeMVPMatrixBuffer(const Flare::RenderSystem::VertexDataLayout &bufferLayout);
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
}

#endif
