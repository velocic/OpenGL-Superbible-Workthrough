#ifndef FLARE_RENDERSYSTEM_TEXTUREMANAGER_H
#define FLARE_RENDERSYSTEM_TEXTUREMANAGER_H

#include <functional>
#include <string>

#include <flare/rendersystem/texture.h>

namespace Flare
{
    namespace RenderSystem
    {
        struct MaterialTextures {
            Texture *baseColor;
            Texture *normal;
            Texture *metallic;
            Texture *roughness;
        };

        class TextureManager
        {
            protected:
                struct MaterialTextures {
                    std::unique_ptr<Texture> baseColor;
                    std::unique_ptr<Texture> normal;
                    std::unique_ptr<Texture> metallic;
                    std::unique_ptr<Texture> roughness;
                };

            public:
                static constexpr RSsizei DEFAULT_NUM_MIPMAP_LEVELS = 4;

                enum class SupportedFileType {
                    PNG
                };

                enum class MaterialTextureType {
                    BASE_COLOR,
                    NORMAL,
                    METALLIC,
                    ROUGHNESS
                };

                struct TextureInitParams {
                    RSsizei numMipmapLevels;
                    RSenum internalFormat;
                    RSboolean generateMipmaps;
                };

                struct TextureFile {
                    std::string path;
                    std::string alias;
                    MaterialTextureType materialTextureType;
                    SupportedFileType fileType;
                    RSenum pixelDataFormat;
                };

                struct ArrayTextureFiles {
                    std::vector<std::string> paths;
                    std::string alias;
                    SupportedFileType type;
                    RSenum pixelDataFormat;
                };

                virtual ~TextureManager() {}

                virtual void batchLoadTexture1D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture2D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;

                virtual void loadTexture1D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::MaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture2D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::MaterialTextures)> onLoadComplete) = 0;
                virtual void loadArrayTexture2D(const ArrayTextureFiles &files, const TextureInitParams &initParams, std::function<void(Texture *)> onLoadComplete) = 0;

                //TODO: support loading in-memory textures (for making runtime-generated textures available)

                virtual RenderSystem::MaterialTextures get(const std::string &alias) const = 0;
                virtual Texture *getArrayTexture(const std::string &alias) const = 0;
                virtual void remove(const std::string &alias) = 0;
                virtual void removeArrayTexture(const std::string &alias) = 0;
                virtual void removeAll() = 0;
        };
    }
}

#endif
