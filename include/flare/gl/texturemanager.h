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
                std::unordered_map<size_t, std::unique_ptr<Texture>> textures;

            public:
                TextureManager() {}
                ~TextureManager();
                TextureManager(TextureManager &&other);
                TextureManager &operator=(TextureManager &&other);
                TextureManager(const TextureManager &other) = delete;
                TextureManager &operator=(const TextureManager &other) = delete;

                virtual void batchLoadTexture1D(const std::vector<TextureFile> &targets, std::function<void()> onLoadComplete) override;
                virtual void batchLoadTexture2D(const std::vector<TextureFile> &targets, std::function<void()> onLoadComplete) override;
                virtual void batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, std::function<void()> onLoadComplete) override;

                virtual void loadTexture1D(const TextureFile &file, std::function<void(Texture *)> onLoadComplete) override;
                virtual void loadTexture2D(const TextureFile &file, std::function<void(Texture *)> onLoadComplete) override;
                virtual void loadArrayTexture2D(const ArrayTextureFiles &files, std::function<void(Texture *)> onLoadComplete) override;

                virtual Texture *get(const std::string &alias) override;
                virtual void remove(const std::string &alias) override;
                virtual void removeAll() override;
        };
    }
}

#endif
