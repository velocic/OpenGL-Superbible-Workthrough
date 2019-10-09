#ifndef TUTORIAL_PACKED_MESH_RENDERING_H
#define TUTORIAL_PACKED_MESH_RENDERING_H

#include <memory>
#include <string>

#include <flare/application.h>
#include <flare/ui/renderwindow.h>
#include <flare/rendersystem/sampler.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/texturemanager.h>
#include <flare/scenegraph/modelmanager.h>

namespace Tutorial
{
    class PackedMeshRendering : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::RenderSystem::ShaderManager> shaderManager = nullptr;
            std::unique_ptr<Flare::RenderSystem::TextureManager> textureManager = nullptr;
            std::unique_ptr<Flare::SceneGraph::ModelManager> modelManager = nullptr;
            std::unique_ptr<Flare::RenderSystem::Buffer> mvpMatrixBuffer = nullptr;
            std::unique_ptr<Flare::RenderSystem::Sampler> diffuse0TextureSampler = nullptr;
            std::unique_ptr<Flare::RenderSystem::Sampler> diffuse1TextureSampler = nullptr;
            std::unique_ptr<Flare::RenderSystem::Sampler> specularTextureSampler = nullptr;
            std::unique_ptr<Flare::RenderSystem::Sampler> normalTextureSampler = nullptr;
            const std::string vertexShaderPath{"../src/7b-packed-mesh-rendering/shaders/vertex.glsl"};
            const std::string fragmentShaderPath{"../src/7b-packed-mesh-rendering/shaders/fragment.glsl"};

            void initializeMVPMatrixBuffer(const Flare::RenderSystem::VertexDataLayout &bufferLayout);
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
}

#endif
