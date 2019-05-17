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

        auto arrayTextureSampler = Flare::GL::Sampler("tex_droplets");
        arrayTextureSampler.samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        arrayTextureSampler.samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        arrayTextureSampler.samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        arrayTextureSampler.samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        dropletShader = Flare::GL::ShaderProgramBuilder()
            .addVertexShader(Utility::File::getFileContents(vertexShaderPath))
            .addFragmentShader(Utility::File::getFileContents(fragmentShaderPath))
            .addTextureUnit(std::move(arrayTextureSampler))
            .build();

        auto numTextureLayers = 100u;
        auto arrayTexture = std::make_shared<Flare::GL::ArrayTexture2D>(8, GL_RGBA8, 256, 256, numTextureLayers);

        {
            auto imageData = std::array<LoadedDropletImageData, 10>();
            for (unsigned int i = 0; i < dropletFiles.size(); ++i) {
                lodepng::decode(imageData[i].rawImageData, imageData[i].imageWidth, imageData[i].imageHeight, dropletFiles[i]);
            }

            for (unsigned int i = 0; i < numTextureLayers; ++i) {
                arrayTexture->textureSubImage3D(
                    0,
                    0, 0,
                    i,
                    256, 256,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    &(imageData[i % 10].rawImageData[0]),
                    true
                );
            }
        }

        dropletShader->setTexture("tex_droplets", arrayTexture);

        auto dropletIndexBufferLayout = Flare::GL::VertexDataLayoutBuilder()
            .addAttribute("droplet_index", 1, GL_INT, GL_FALSE, 0)
            .setStride(sizeof(int))
            .build();

        dropletIndexBuffer = std::make_unique<Flare::GL::Buffer>(dropletIndexBufferLayout);
        dropletIndexBuffer->namedBufferStorage(sizeof(int), nullptr, GL_DYNAMIC_STORAGE_BIT);

        basicVAO = std::make_unique<Flare::GL::VertexArray>(
            *(dropletShader.get()),
            std::vector<std::reference_wrapper<const Flare::GL::Buffer>>{
                *(dropletIndexBuffer.get())
            }
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
        auto t = static_cast<float>(elapsedTime * .003);

        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        auto &[dropletBuffer, dropletArrayHandle] = dropletUniformBufferObject->getHandles();

        dropletUniformBufferObject->bindBufferBase(0);

        for (unsigned int i = 0; i < 256; ++i) {
            dropletArrayHandle[i].x_offset = dropletXOffset[i];
            dropletArrayHandle[i].y_offset = 2.0f - fmodf((t + static_cast<float>(i)) * dropletFallSpeed[i], 4.31f);
            dropletArrayHandle[i].orientation = t * dropletRotSpeed[i];
            dropletArrayHandle[i].padding = 0.0f;
        }

        dropletUniformBufferObject->namedBufferData(GL_DYNAMIC_DRAW);

        glClearBufferfv(GL_COLOR, 0, clearColor);
        for (unsigned int dropletIndex = 0; dropletIndex < 256; ++dropletIndex) {
            dropletIndexBuffer->namedBufferSubData(0, sizeof(int), &dropletIndex);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        SDL_GL_SwapWindow(renderWindow->getRenderWindowHandle());
    }

    void ArrayTextures::shutdown()
    {
        renderWindow->freeResources();
    }
}
