#ifndef TUTORIAL_ARRAYTEXTURES_H
#define TUTORIAL_ARRAYTEXTURES_H

#include <memory>
#include <string>

#include <GL/gl3w.h>

#include <flare/gl/uniformblock.h>
#include <flare/application.h>
#include <flare/gl/buffer.h>
#include <flare/gl/vertexarray.h>
#include <flare/gl/shaderprogram.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class ArrayTextures : public Flare::Application
    {
        private:
            struct LoadedDropletImageData {
                unsigned int imageWidth = 0;
                unsigned int imageHeight = 0;
                std::vector<unsigned char> rawImageData;
            };

            struct DropletInstanceData {
                float x_offset = 0;
                float y_offset = 0;
                float orientation = 0;
                float padding = 0;
            };

            std::array<float, 256> dropletXOffset{}; 
            std::array<float, 256> dropletRotSpeed{};
            std::array<float, 256> dropletFallSpeed{};

            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::GL::VertexArray> basicVAO = nullptr;
            std::unique_ptr<Flare::GL::ShaderProgram> dropletShader = nullptr;
            decltype(Flare::GL::buildStd140AlignedUniformBlockBuffer(Flare::GL::GLSLArrayType<DropletInstanceData, 256>{})) dropletUniformBufferObject;

            const std::array<std::string, 10> dropletFiles{
                "../src/5c-array-textures/textures/test1.png",
                "../src/5c-array-textures/textures/test2.png",
                "../src/5c-array-textures/textures/test3.png",
                "../src/5c-array-textures/textures/test4.png",
                "../src/5c-array-textures/textures/test5.png",
                "../src/5c-array-textures/textures/test6.png",
                "../src/5c-array-textures/textures/test7.png",
                "../src/5c-array-textures/textures/test8.png",
                "../src/5c-array-textures/textures/test9.png",
                "../src/5c-array-textures/textures/test10.png"
            };

            const std::string vertexShaderPath = "../src/5c-array-textures/shaders/vertex.glsl";
            const std::string fragmentShaderPath = "../src/5c-array-textures/shaders/fragment.glsl";
            unsigned int elapsedTime = 0;

            void initializeRandomizedDropletInstanceParams();

        public:
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;
    };
}

#endif
