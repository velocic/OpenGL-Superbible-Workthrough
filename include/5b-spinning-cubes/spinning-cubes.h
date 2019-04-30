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
        public:
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;
    };
};

#endif
