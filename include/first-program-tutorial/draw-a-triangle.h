#ifndef TUTORIAL_DRAW_A_TRIANGLE_H
#define TUTORIAL_DRAW_A_TRIANGLE_H

#include <memory>
#include <string>

#include <GL/gl3w.h>
#include <flare/application.h>
#include <flare/gl/shaderprogram.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class DrawATriangle : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::GL::ShaderProgram> plainTriangleShader = nullptr;
            const std::string vertexShaderPath;
            const std::string fragmentShaderPath;
            GLuint VAO = 0;
        public:
            DrawATriangle();
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;

    };
}

#endif
