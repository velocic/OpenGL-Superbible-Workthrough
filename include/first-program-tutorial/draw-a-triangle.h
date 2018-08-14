#ifndef TUTORIAL_DRAW_A_TRIANGLE_H
#define TUTORIAL_DRAW_A_TRIANGLE_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/application.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class DrawATriangle : Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
        public:
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;

    };
}

#endif
