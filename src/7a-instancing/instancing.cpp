#include <7a-instancing/instancing.h>

#include <flare/gl/texturemanager.h>

namespace Tutorial
{
    void Instancing::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Instanced models",
            1280,
            720
        );

        textureManager = std::make_unique<Flare::GL::TextureManager>();
        modelManager = std::make_unique<Flare::SceneGraph::ModelManager>(*textureManager.get());
        shaderManager = std::make_unique<Flare::RenderSystem::ShaderManager>();
    }

    void Instancing::render(unsigned int deltaTime)
    {
    }

    void Instancing::shutdown()
    {
    }
}
