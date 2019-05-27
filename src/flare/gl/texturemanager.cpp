#include <flare/gl/texturemanager.h>

#include <lodepng/lodepng.h>

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
            throw std::runtime_error("GL::TextureManager::batchLoadTexture1D() not yet implemented.");
        }

        void TextureManager::batchLoadTexture2D(const std::vector<TextureFile> &targets, std::function<void()> onLoadComplete)
        {
        }

        void TextureManager::batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, std::function<void()> onLoadComplete)
        {
        }

        void TextureManager::loadTexture1D(const TextureFile &file, std::function<void(RenderSystem::Texture *)> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::loadTexture1D() not yet implemented.");
        }

        void TextureManager::loadTexture2D(const TextureFile &file, std::function<void(RenderSystem::Texture *)> onLoadComplete)
        {
            auto width = 0u;
            auto height = 0u;
            auto decodedImage = std::vector<unsigned char>{};

            lodepng::decode(decodedImage, width, height, file.path);

            auto newTexture = std::make_unique<Texture2D>();
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
