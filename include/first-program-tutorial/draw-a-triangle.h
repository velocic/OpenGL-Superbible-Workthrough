#ifndef TUTORIAL_DRAW_A_TRIANGLE_H
#define TUTORIAL_DRAW_A_TRIANGLE_H

#include <memory>
#include <string>

#include <GL/gl3w.h>
#include <flare/application.h>
#include <flare/material/material.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class DrawATriangle : Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::Material> plainTriangleShader = nullptr;
            const std::string vertexShaderPath;
            const std::string fragmentShaderPath;
        public:
            DrawATriangle();
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;

    };
}

#endif
