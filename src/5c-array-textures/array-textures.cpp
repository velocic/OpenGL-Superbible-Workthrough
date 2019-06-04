#include <5c-array-textures/array-textures.h>

#include <flare/gl/arraytexture2d.h>
#include <flare/utility/file.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>
#include <lodepng/lodepng.h>

namespace Tutorial
{
    //Random generation of floats from OpenGL Superbible sb7code alienrain.cpp
    static unsigned int seed = 0x13371337;
    static inline float randomFloat()
    {
        float res;
        unsigned int tmp;

        seed *= 16807;

        tmp = seed ^ (seed >> 4) ^ (seed << 15);

        *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

        return (res - 1.0f);
    }

    void ArrayTextures::initializeRandomizedDropletInstanceParams()
    {
        for (unsigned int i = 0; i < 256; ++i) {
            dropletXOffset[i] = randomFloat() * 2.0f - 1.0f;
            dropletRotSpeed[i] = (randomFloat() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
            dropletFallSpeed[i] = randomFloat() + 0.2f;
        }
    }

    void ArrayTextures::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Raining 2DArrayTexture Droplets",
            1280,
            720
        );

        auto arrayTextureSampler = std::make_unique<Flare::GL::Sampler>("tex_droplets");
        arrayTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        arrayTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        arrayTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        arrayTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        dropletShader = Flare::GL::ShaderProgramBuilder()
            .setVertexShader(vertexShaderPath)
            .setFragmentShader(fragmentShaderPath)
            .addTextureUnit(arrayTextureSampler.get())
            .build();

        textureManager->loadArrayTexture2D(
            Flare::RenderSystem::TextureManager::ArrayTextureFiles{
                dropletFiles,
                "testDropletImages",
                Flare::RenderSystem::TextureManager::SupportedFileType::PNG,
                GL_RGBA
            },
            Flare::RenderSystem::TextureManager::TextureInitParams{
                8,
                GL_RGBA8,
                true
            },
            [&](auto loadedArrayTexture){
                dropletShader->setTexture(arrayTextureSampler->getName(), loadedArrayTexture);
            }
        );

        basicVAO = std::make_unique<Flare::GL::VertexArray>(
            *(dropletShader.get()),
            std::vector<std::reference_wrapper<const Flare::RenderSystem::Buffer>>{}
        );
        basicVAO->bind();
        dropletShader->bind();

        dropletUniformBufferObject = Flare::GL::buildStd140AlignedUniformBlockBuffer(
            Flare::GL::GLSLArrayType<DropletInstanceData, 256>{}
        );

        initializeRandomizedDropletInstanceParams();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void ArrayTextures::render(unsigned int deltaTime)
    {
        elapsedTime += deltaTime;
        auto t = static_cast<float>(elapsedTime * .0015);

        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        auto &[dropletBuffer, dropletArrayHandle] = dropletUniformBufferObject->getHandles();

        dropletUniformBufferObject->bindBufferBase(0);

        for (unsigned int i = 0; i < 256; ++i) {
            dropletArrayHandle[i].x_offset = dropletXOffset[i];
            dropletArrayHandle[i].y_offset = 2.0f - fmodf((t + static_cast<float>(i)) * dropletFallSpeed[i], 4.31f);
            dropletArrayHandle[i].orientation = t * dropletRotSpeed[i];
            dropletArrayHandle[i].padding = 0.0f;
        }

        dropletUniformBufferObject->namedBufferSubData();

        glClearBufferfv(GL_COLOR, 0, clearColor);

        for (int dropletIndex = 0; dropletIndex < 256; ++dropletIndex) {
            glVertexAttribI1i(0, dropletIndex);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        renderWindow->swapWindow();
    }

    void ArrayTextures::shutdown()
    {
        renderWindow->freeResources();
    }
}
