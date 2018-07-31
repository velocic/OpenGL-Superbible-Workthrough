#ifndef TUTORIAL_DRAW_A_TRIANGLE_H
#define TUTORIAL_DRAW_A_TRIANGLE_H

#include <GL/gl3w.h>
#include <flare/application.h>

namespace Tutorial
{
    class DrawATriangle : Flare::Application
    {
        private:
        public:
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;

    };
}

#endif
