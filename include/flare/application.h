#ifndef FLARE_APPLICATION_H
#define FLARE_APPLICATION_H

namespace Flare
{
    class Application
    {
        private:
        public:
            virtual void initialize() = 0;
            virtual void render(unsigned int deltaTime) = 0;
            virtual void shutdown() = 0;
    };
}

#endif
