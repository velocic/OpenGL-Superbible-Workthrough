#ifndef FLARE_GL_TEXTUREMANAGER_H
#define FLARE_GL_TEXTUREMANAGER_H

#include <memory>
#include <unordered_map>

#include <flare/gl/texture.h>
#include <flare/rendersystem/texturemanager.h>

namespace Flare
{
    namespace GL
    {
        class TextureManager : public RenderSystem::TextureManager
        {
            private:
                std::unordered_map<size_t, PBRMaterialTextures> PBRTextures;
                std::unordered_map<size_t, PhongMaterialTextures> phongTextures;
                std::unordered_map<size_t, std::unique_ptr<Texture>> arrayTextures;
                std::unordered_map<size_t, std::unique_ptr<Texture>> textures;
                std::hash<std::string> stringHasher;

                RenderSystem::PBRMaterialTextures getNonOwningMaterialPointers(const PBRMaterialTextures &owningPointers) const;
                RenderSystem::PhongMaterialTextures getNonOwningMaterialPointers(const PhongMaterialTextures &owningPointers) const;
            public:
                TextureManager() {}
                ~TextureManager();
                TextureManager(TextureManager &&other);
                TextureManager &operator=(TextureManager &&other);
                TextureManager(const TextureManager &other) = delete;
                TextureManager &operator=(const TextureManager &other) = delete;

                virtual void batchLoadTexture1D(const std::vector<PBRTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) override;
                virtual void batchLoadTexture1D(const std::vector<PhongTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) override;
                virtual void batchLoadTexture1D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) override;
                virtual void batchLoadTexture2D(const std::vector<PBRTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) override;
                virtual void batchLoadTexture2D(const std::vector<PhongTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) override;
                virtual void batchLoadTexture2D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) override;
                virtual void batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) override;

                virtual void loadTexture1D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete) override;
                virtual void loadTexture1D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete) override;
                virtual void loadTexture1D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete) override;
                virtual void loadTexture2D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete) override;
                virtual void loadTexture2D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete) override;
                virtual void loadTexture2D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete) override;
                virtual void loadArrayTexture2D(const ArrayTextureFiles &files, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete) override;

                virtual RenderSystem::Texture *get(const std::string &alias) const override;
                virtual RenderSystem::PBRMaterialTextures getPBRMaterialTextures(const std::string &alias) const override;
                virtual RenderSystem::PhongMaterialTextures getPhongMaterialTextures(const std::string &alias) const override;
                virtual RenderSystem::Texture *getArrayTexture(const std::string &alias) const override;
                virtual bool arePBRMaterialTexturesLoaded(const std::string &alias) const override;
                virtual bool arePhongMaterialTexturesLoaded(const std::string &alias) const override;
                virtual void remove(const std::string &alias) override;
                virtual void removePBRMaterialTextures(const std::string &alias) override;
                virtual void removePhongMaterialTextures(const std::string &alias) override;
                virtual void removeArrayTexture(const std::string &alias) override;
                virtual void removeAll() override;
        };
    }
}

#endif
