#ifndef FLARE_RENDERSYSTEM_TEXTUREMANAGER_H
#define FLARE_RENDERSYSTEM_TEXTUREMANAGER_H

#include <functional>
#include <string>

#include <flare/rendersystem/texture.h>

namespace Flare
{
    namespace RenderSystem
    {
        struct PBRMaterialTextures {
            Texture *baseColor;
            Texture *normal;
            Texture *metallic;
            Texture *roughness;
        };

        struct PhongMaterialTextures {
            Texture *diffuse;
            Texture *specular;
            Texture *normal;
        };

        class TextureManager
        {
            protected:
                struct PBRMaterialTextures {
                    std::unique_ptr<Texture> baseColor;
                    std::unique_ptr<Texture> normal;
                    std::unique_ptr<Texture> metallic;
                    std::unique_ptr<Texture> roughness;
                };

                struct PhongMaterialTextures {
                    std::unique_ptr<Texture> diffuse;
                    std::unique_ptr<Texture> specular;
                    std::unique_ptr<Texture> normal;
                };

            public:
                static constexpr RSsizei DEFAULT_NUM_MIPMAP_LEVELS = 4;

                enum class SupportedFileType {
                    PNG
                };

                enum class PBRMaterialTextureType {
                    BASE_COLOR,
                    NORMAL,
                    METALLIC,
                    ROUGHNESS
                };

                enum class PhongMaterialTextureType {
                    DIFFUSE,
                    SPECULAR,
                    NORMAL
                };

                struct TextureInitParams {
                    RSsizei numMipmapLevels;
                    RSenum internalFormat;
                    RSboolean generateMipmaps;
                };

                struct PBRTextureFile {
                    std::string path;
                    std::string alias;
                    PBRMaterialTextureType materialTextureType;
                    SupportedFileType fileType;
                    RSenum pixelDataFormat;
                };

                struct PhongTextureFile {
                    std::string path;
                    std::string alias;
                    PhongMaterialTextureType materialTextureType;
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

                virtual void batchLoadTexture1D(const std::vector<PBRTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture1D(const std::vector<PhongTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture2D(const std::vector<PBRTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture2D(const std::vector<PhongTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;

                virtual void loadTexture1D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture1D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture2D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture2D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete) = 0;
                virtual void loadArrayTexture2D(const ArrayTextureFiles &files, const TextureInitParams &initParams, std::function<void(Texture *)> onLoadComplete) = 0;

                //TODO: support loading in-memory textures (for making runtime-generated textures available)

                virtual RenderSystem::PBRMaterialTextures get(const std::string &alias) const = 0;
                virtual RenderSystem::PhongMaterialTextures getPhongMaterialTextures(const std::string &alias) const = 0;
                virtual Texture *getArrayTexture(const std::string &alias) const = 0;
                virtual void remove(const std::string &alias) = 0;
                virtual void removePhongMaterialTextures(const std::string &alias) = 0;
                virtual void removeArrayTexture(const std::string &alias) = 0;
                virtual void removeAll() = 0;
        };
    }
}

#endif
