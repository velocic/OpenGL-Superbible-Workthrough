#include <chrono>
#include <thread>

#include <first-program-tutorial/draw-a-triangle.h>
#include <3-gpu-shader-pipeline/shader-pipeline.h>
#include <5-data/data.h>
#include <5b-spinning-cubes/spinning-cubes.h>
#include <5c-array-textures/array-textures.h>
#include <6a-model-file-loading/model-file-loading.h>

#include <flare/gl/uniformblock.h>

int main()
{
    unsigned int numFramesToRender = 10 * 60;
    auto targetFrameTimeMillis = 16.6;

    // Tutorial::DrawATriangle selectedTutorial;
    // Tutorial::ShaderPipeline selectedTutorial;
    // Tutorial::Data selectedTutorial;
    // Tutorial::SpinningCubes selectedTutorial;
    // Tutorial::ArrayTextures selectedTutorial;
    Tutorial::ModelFileLoading selectedTutorial;

    Flare::Application* demoApp = &selectedTutorial;

    demoApp->initialize();

    //Fake a render loop at slow enough FPS to not blind myself
    auto previousFrameStartTime = std::chrono::high_resolution_clock::now();
    for (unsigned int i = 0; i < numFramesToRender; ++i) {
        auto currentFrameStartTime = std::chrono::high_resolution_clock::now();
        auto timeSinceLastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameStartTime - previousFrameStartTime).count();

        demoApp->render(timeSinceLastFrame);

        long sleepTime = targetFrameTimeMillis - timeSinceLastFrame;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

        previousFrameStartTime = currentFrameStartTime;
    }

    demoApp->shutdown();

    return 0;
}
