#ifndef FLARE_RENDERWINDOW_H
#define FLARE_RENDERWINDOW_H

#include <SDL2/SDL.h>
#include <GL/gl3w.h>

#include <string>
#include <iostream>

namespace Flare
{
    class RenderWindow
    {
        private:
            SDL_Window* window = nullptr;
            SDL_GLContext renderContext;
            float aspectRatio;
        public:
            RenderWindow(const std::string& windowTitle, unsigned int screenWidth, unsigned int screenHeight);
            ~RenderWindow();
            float getAspectRatio() {return aspectRatio;};
            void freeResources();
            SDL_Window* getRenderWindowHandle();
    };
};

#endif
