#ifndef TUTORIAL_MODELFILELOADING_H
#define TUTORIAL_MODELFILELOADING_H

#include <vector>
#include <memory>

#include <flare/application.h>
#include <flare/ui/renderwindow.h>
#include <flare/rendersystem/shaderprogram.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/modelmanager.h>

namespace Tutorial
{
    class ModelFileLoading : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::RenderSystem::ShaderProgram> basicUnlitMeshDisplayShader = nullptr;
            std::unique_ptr<Flare::RenderSystem::Sampler> diffuseTextureSampler = nullptr;
            std::unique_ptr<Flare::RenderSystem::Sampler> specularTextureSampler = nullptr;
            std::unique_ptr<Flare::RenderSystem::TextureManager> textureManager = nullptr;
            std::unique_ptr<Flare::SceneGraph::ModelManager> modelManager = nullptr;
            std::unique_ptr<Flare::RenderSystem::Buffer> mvpMatrixBuffer = nullptr;
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
};

#endif
