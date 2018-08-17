#ifndef TUTORIAL_SHADER_PIPELINE_H
#define TUTORIAL_SHADER_PIPELINE_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/application.h>
#include <flare/material/material.h>
#include <flare/ui/renderwindow.h>

namespace Tutorial
{
    class ShaderPipeline : public Flare::Application
    {
        private:
            std::unique_ptr<Flare::RenderWindow> renderWindow = nullptr;
            std::unique_ptr<Flare::Material> shaderPipelineDemoShader = nullptr;
            const std::string vertexShaderPath{"../src/3-gpu-shader-pipeline/shaders/vertex.vsh"};
            const std::string fragmentShaderPath{"../src/3-gpu-shader-pipeline/shaders/fragment.fsh"};
            unsigned long long elapsedTime = 0;
            GLuint VAO = 0;
        public:
            void initialize() override;
            void render(unsigned int deltaTime) override;
            void shutdown() override;
    };
};

#endif
