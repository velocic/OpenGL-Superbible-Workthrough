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
            std::unique_ptr<Flare::RenderSystem::ShaderProgram> plainTriangleShader = nullptr;
            const std::string vertexShaderPath;
            const std::string fragmentShaderPath;
            GLuint VAO = 0;
        public:
            DrawATriangle();
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;

    };
}

#endif
