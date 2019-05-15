#include <5c-array-textures/array-textures.h>

#include <flare/gl/arraytexture2d.h>
#include <flare/gl/uniformblock.h>
#include <flare/utility/file.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>
#include <lodepng/lodepng.h>

namespace Tutorial
{
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
                    false
                );
            }
        }

        dropletShader->setTexture("tex_droplets", arrayTexture);

        //create basic VAO with no associated buffers and bind. I believe it's necessary to have
        //a VAO bound to do rendering at all, even if using no buffers
        basicVAO = std::make_unique<Flare::GL::VertexArray>(
            *(dropletShader.get()),
            std::vector<std::reference_wrapper<const Flare::GL::Buffer>>()
        );
        basicVAO->bind();
        dropletShader->bind();

        //create uniform buffer object, fill with droplet instance data
        // auto [dropletInstanceDataBuffer, dropletInstanceDataHandle] = Flare::GL::buildStd140AlignedUniformBlockBuffer(
        //     Flare::GL::GLSLType<glm::vec4>{}
        // );

        //DEBUG
        Flare::GL::buildStd140AlignedUniformBlockBuffer(
            Flare::GL::GLSLType<glm::vec4>{}
        );
        //END DEBUG
    }

    void ArrayTextures::render(unsigned int deltaTime)
    {
        const GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, clearColor);

        //TODO: add render loop logic, place data into UBO array

        SDL_GL_SwapWindow(renderWindow->getRenderWindowHandle());
    }

    void ArrayTextures::shutdown()
    {
        renderWindow->freeResources();
    }
}
