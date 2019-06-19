#include <6a-model-file-loading/model-file-loading.h>

#include <flare/gl/texturemanager.h>
#include <flare/gl/sampler.h>

namespace Tutorial
{
    void ModelFileLoading::initialize()
    {
        renderWindow = std::make_unique<Flare::RenderWindow>(
            "Loading a 3D model file",
            1280,
            720
        );

        textureManager = std::make_unique<Flare::GL::TextureManager>();
        diffuseTextureSampler = std::make_unique<Flare::GL::Sampler>("diffuse_texture");
        specularTextureSampler = std::make_unique<Flare::GL::Sampler>("specular_texture");

        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        diffuseTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        specularTextureSampler->samplerParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        specularTextureSampler->samplerParameteri(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }

    void ModelFileLoading::render(unsigned int deltaTime)
    {
    }

    void ModelFileLoading::shutdown()
    {
    }
}
