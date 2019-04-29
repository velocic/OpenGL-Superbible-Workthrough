#include <chrono>
#include <thread>

#include <first-program-tutorial/draw-a-triangle.h>
#include <3-gpu-shader-pipeline/shader-pipeline.h>
#include <5-data/data.h>
#include <flare/gl/datalayout.h>

#include <flare/gl/uniformblock.h>

int main(int argc, char* argv[])
{
    unsigned int numFramesToRender = 10 * 60;
    auto targetFrameTimeMillis = 16.6;

    // Tutorial::DrawATriangle selectedTutorial;
    // Tutorial::ShaderPipeline selectedTutorial;
    Tutorial::Data selectedTutorial;

    Flare::Application* demoApp = &selectedTutorial;

    //DEBUG
    // auto&& [testBuffer, testFloat, testVec3, testMat4, testFloatArray3, testBool, testInt] = Flare::GL::buildStd140AlignedUniformBlockBuffer(
    //     Flare::GL::GLSLType<float>{},
    //     Flare::GL::GLSLType<glm::vec3>{},
    //     Flare::GL::GLSLType<glm::mat4>{},
    //     Flare::GL::GLSLArrayType<float, 3>{},
    //     Flare::GL::GLSLType<bool>{},
    //     Flare::GL::GLSLType<int>{}
    // );

    // auto testBlock = UniformBlock(
    //     GLSLType<float>{},
    //     GLSLType<glm::vec3>{},
    //     GLSLType<glm::mat4>{},
    //     GLSLArrayType<float, 3>{},
    //     GLSLType<bool>{},
    //     GLSLType<int>{}
    // );
    auto testBlock = Flare::GL::buildStd140AlignedUniformBlockBuffer(
        Flare::GL::GLSLType<float>{},
        Flare::GL::GLSLType<glm::vec3>{},
        Flare::GL::GLSLType<glm::mat4>{},
        Flare::GL::GLSLArrayType<float, 3>{},
        Flare::GL::GLSLType<bool>{},
        Flare::GL::GLSLType<int>{}
    );

    auto&& [testBuffer, testFloat, testVec3, testMat4, testFloatArray3, testBool, testInt] = testBlock;
    //
    *testFloat = 32.5f;
    *testVec3 = glm::vec3(-25, 25, 125);
    *testMat4 = glm::mat4(
        10, 10, 10, 10,
        20, 20, 20, 20,
        30, 30, 30, 30,
        40, 40, 40, 40
    );
    testFloatArray3[0] = 25.0f;
    testFloatArray3[1] = 50.2f;
    testFloatArray3[2] = 75.5f;
    *testBool = true;
    *testInt = ~0;

    // auto testBlock = Flare::GL::buildStd140AlignedUniformBlockBuffer(
    //     Flare::GL::GLSLType<float>{},
    //     Flare::GL::GLSLType<glm::vec3>{},
    //     Flare::GL::GLSLType<glm::mat4>{},
    //     Flare::GL::GLSLArrayType<float, 3>{},
    //     Flare::GL::GLSLType<bool>{},
    //     Flare::GL::GLSLType<int>{}
    // );
    int debug = 5;
    //END DEBUG

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
