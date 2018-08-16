#include <3-gpu-shader-pipeline/shader-pipeline.h>
#include <flare/utility/file.h>

namespace Tutorial
{
    void ShaderPipeline::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Demonstrating the Programmable Pipeline",
            1920,
            1080
        );

        std::vector<uint8_t> vertexShaderSource;
        std::vector<uint8_t> fragmentShaderSource;

        Utility::File::getFileContents(vertexShaderSource, vertexShaderPath);
        Utility::File::getFileContents(fragmentShaderSource, fragmentShaderPath);
    }

    void ShaderPipeline::render(unsigned int deltaTime)
    {
    }

    void ShaderPipeline::shutdown()
    {
    }
}
