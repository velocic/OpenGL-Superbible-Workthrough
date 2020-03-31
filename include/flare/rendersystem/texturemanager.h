#ifndef FLARE_RENDERSYSTEM_TEXTUREMANAGER_H
#define FLARE_RENDERSYSTEM_TEXTUREMANAGER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <flare/rendersystem/texture.h>

namespace Flare
{
    namespace RenderSystem
    {
        struct PBRMaterialTextures {
            std::vector<Texture *> baseColor;
            std::vector<Texture *> normal;
            std::vector<Texture *> metallic;
            std::vector<Texture *> roughness;
            size_t materialId = 0;
        };

        struct PhongMaterialTextures {
            std::vector<Texture *> diffuse;
            std::vector<Texture *> specular;
            std::vector<Texture *> normal;
            size_t materialId = 0;
        };

        class TextureManager
        {
            protected:
                struct PBRMaterialTextures {
                    std::vector<std::unique_ptr<Texture>> baseColor;
                    std::vector<std::unique_ptr<Texture>> normal;
                    std::vector<std::unique_ptr<Texture>> metallic;
                    std::vector<std::unique_ptr<Texture>> roughness;
                    size_t materialId = 0;
                };

                struct PhongMaterialTextures {
                    std::vector<std::unique_ptr<Texture>> diffuse;
                    std::vector<std::unique_ptr<Texture>> specular;
                    std::vector<std::unique_ptr<Texture>> normal;
                    size_t materialId = 0;
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

                struct TextureFile {
                    std::string path;
                    std::string alias;
                    SupportedFileType fileType;
                    RSenum pixelDataFormat;
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

                virtual void batchLoadTexture1D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture2D(const std::vector<PBRTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture2D(const std::vector<PhongTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture2D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) = 0;

                virtual void loadTexture1D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture1D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture1D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete) = 0;
                virtual void loadTexture2D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture2D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete) = 0;
                virtual void loadTexture2D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete) = 0;
                virtual void loadArrayTexture2D(const ArrayTextureFiles &files, const TextureInitParams &initParams, std::function<void(Texture *)> onLoadComplete) = 0;

                //TODO: support loading in-memory textures (for making runtime-generated textures available)

                virtual Texture *get(const std::string &alias) const = 0;
                virtual RenderSystem::PBRMaterialTextures getPBRMaterialTextures(const std::string &alias) const = 0;
                virtual RenderSystem::PhongMaterialTextures getPhongMaterialTextures(const std::string &alias) const = 0;
                virtual Texture *getArrayTexture(const std::string &alias) const = 0;
                virtual bool arePBRMaterialTexturesLoaded(const std::string &alias) const = 0;
                virtual bool arePhongMaterialTexturesLoaded(const std::string &alias) const = 0;
                virtual void remove(const std::string &alias) = 0;
                virtual void removePBRMaterialTextures(const std::string &alias) = 0;
                virtual void removePhongMaterialTextures(const std::string &alias) = 0;
                virtual void removeArrayTexture(const std::string &alias) = 0;
                virtual void removeAll() = 0;
        };
    }
}

#endif
