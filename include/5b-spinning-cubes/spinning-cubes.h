#ifndef TUTORIAL_SPINNINGCUBES_H
#define TUTORIAL_SPINNINGCUBES_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/application.h>
#include <flare/gl/buffer.h>
#include <flare/gl/vertexarray.h>
#include <flare/gl/shaderprogram.h>
#include <flare/gl/uniformblock.h>
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
            const std::string vertexShaderPath{"../src/5b-spinning-cubes/shaders/vertex.glsl"};
            const std::string fragmentShaderPath{"../src/5b-spinning-cubes/shaders/fragment.glsl"};
            std::array<GLfloat, 36> cubeVertexPositions;
        public:
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;
    };
};

#endif
