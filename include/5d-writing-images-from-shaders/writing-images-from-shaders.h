#ifndef TUTORIAL_WRITINGIAMGESFROMSHADERS_H
#define TUTORIAL_WRITINGIAMGESFROMSHADERS_H

#include <memory>
#include <string>

#include <GL/gl3w.h>

#include <flare/application.h>

namespace Tutorial
{
    class WritingImagesFromShaders : public Flare::Application
    {
        private:
        public:
            virtual void initialize() override;
            virtual void render(unsigned int deltaTime) override;
            virtual void shutdown() override;
    };
}

#endif
