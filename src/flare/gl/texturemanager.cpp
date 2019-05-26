#include <flare/gl/texturemanager.h>

namespace Flare
{
    namespace GL
    {
        TextureManager::~TextureManager()
        {
            removeAll();
        }

        TextureManager::TextureManager(TextureManager &&other)
        : 
            textures(std::move(other.textures))
        {}

        TextureManager &TextureManager::operator=(TextureManager &&other)
        {
            textures = std::move(other.textures);
        }

        void TextureManager::batchLoadTexture1D(const std::vector<TextureFile> &targets, std::function<void()> onLoadComplete)
        {
        }

        void TextureManager::batchLoadTexture2D(const std::vector<TextureFile> &targets, std::function<void()> onLoadComplete)
        {
        }

        void TextureManager::batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, std::function<void()> onLoadComplete)
        {
        }

        void TextureManager::loadTexture1D(const TextureFile &file, std::function<void(RenderSystem::Texture *)> onLoadComplete)
        {
        }

        void TextureManager::loadTexture2D(const TextureFile &file, std::function<void(RenderSystem::Texture *)> onLoadComplete)
        {
        }

        void TextureManager::loadArrayTexture2D(const ArrayTextureFiles &files, std::function<void(RenderSystem::Texture *)> onLoadComplete)
        {
        }

        Texture *TextureManager::get(const std::string &alias)
        {
        }

        void TextureManager::remove(const std::string &alias)
        {
        }

        void TextureManager::removeAll()
        {
            for (const auto &it : textures) {
                it.second->destroy();
            }

            textures.clear();
        }
    }
}
