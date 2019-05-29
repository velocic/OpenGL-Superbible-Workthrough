#ifndef TUTORIAL_SPINNINGCUBES_H
#define TUTORIAL_SPINNINGCUBES_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/application.h>
#include <flare/gl/buffer.h>
#include <flare/gl/vertexarray.h>
#include <flare/gl/shaderprogram.h>
#include <flare/gl/texturemanager.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class SpinningCubes : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::GL::Buffer> cubeMeshBuffer = nullptr;
            std::unique_ptr<Flare::GL::VertexArray> cubeMeshVAO = nullptr;
            std::unique_ptr<Flare::GL::ShaderProgram> spinningCubeShader = nullptr;
            std::unique_ptr<Flare::RenderSystem::TextureManager> textureManager = std::make_unique<Flare::GL::TextureManager>();
            const std::string vertexShaderPath{"../src/5b-spinning-cubes/shaders/vertex.glsl"};
            const std::string fragmentShaderPath{"../src/5b-spinning-cubes/shaders/fragment.glsl"};
            std::array<GLfloat, 108> cubeVertexPositions;

            unsigned int elapsedTime = 0;
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
};

#endif
