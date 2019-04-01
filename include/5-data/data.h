#ifndef TUTORIAL_DATA_H
#define TUTORIAL_DATA_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/application.h>
#include <flare/gl/buffer.h>
#include <flare/gl/vertexarray.h>
#include <flare/gl/shaderprogram.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class Data : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::GL::ShaderProgram> shaderPipelineDemoShader = nullptr;
            std::unique_ptr<Flare::GL::VertexArray> shaderPipelineDemoShaderVAO = nullptr;
            std::unique_ptr<Flare::GL::Buffer> vertexPositionBuffer = nullptr;
            std::unique_ptr<Flare::GL::Buffer> vertexOffsetBuffer = nullptr;
            std::unique_ptr<Flare::GL::Buffer> triangleColorBuffer = nullptr;
            const std::string vertexShaderPath{"../src/5-data/shaders/vertex.vsh"};
            const std::string fragmentShaderPath{"../src/5-data/shaders/fragment.fsh"};
            const std::string tessellationControlShaderPath{"../src/5-data/shaders/tessellation-control.glsl"};
            const std::string tessellationEvaluationShaderPath{"../src/5-data/shaders/tessellation-evaluation.glsl"};
            const std::string geometryShaderPath{"../src/5-data/shaders/geometry.glsl"};
            unsigned long long elapsedTime = 0;
            GLuint VAO = 0;
        public:
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;
    };
};

#endif
