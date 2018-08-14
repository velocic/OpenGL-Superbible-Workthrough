#include <chrono>
#include <thread>

#include <first-program-tutorial/draw-a-triangle.h>

int main(int argc, char* argv[])
{
    unsigned int numFramesToRender = 30;

    Tutorial::DrawATriangle firstProgramDrawTriangle;
    firstProgramDrawTriangle.initialize();

    //Fake a render loop at slow enough FPS to not blind myself
    unsigned long long elapsedTime = 0;
    for (unsigned int i = 0; i < numFramesToRender; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        firstProgramDrawTriangle.render(elapsedTime);

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        elapsedTime += duration;
    }

    firstProgramDrawTriangle.shutdown();

    return 0;
}
