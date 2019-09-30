#include <flare/gl/texturemanager.h>

#include <lodepng/lodepng.h>

#include <flare/gl/texture2d.h>
#include <flare/gl/arraytexture2d.h>

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

            return *this;
        }

        void TextureManager::batchLoadTexture1D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::batchLoadTexture1D() not yet implemented.");
        }

        void TextureManager::batchLoadTexture2D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            for (const auto &target : targets) {
                loadTexture2D(target, initParams,[](auto){});
            }

            onLoadComplete();
        }

        void TextureManager::batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            for (const auto &target : targets) {
                loadArrayTexture2D(target, initParams, [](auto){});
            }

            onLoadComplete();
        }

        void TextureManager::loadTexture1D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::MaterialTextures)> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::loadTexture1D() not yet implemented.");
        }

        void TextureManager::loadTexture2D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::MaterialTextures)> onLoadComplete)
        {
            auto width = 0u;
            auto height = 0u;
            auto decodedImage = std::vector<unsigned char>{};
            auto lookupKey = stringHasher(file.alias);

            lodepng::decode(decodedImage, width, height, file.path);

            auto newTexture = std::make_unique<Texture2D>(
                initParams.numMipmapLevels,
                initParams.internalFormat,
                width,
                height
            );
            newTexture->bufferPixelData(file.pixelDataFormat, GL_UNSIGNED_BYTE, decodedImage.data(), initParams.generateMipmaps);

            auto entryExists = textures.find(lookupKey) != textures.end();
            if (!entryExists) {
                textures.insert_or_assign(lookupKey, MaterialTextures{});
            }
            auto &entry = textures.find(lookupKey)->second;

            if (file.materialTextureType == MaterialTextureType::BASE_COLOR) {
                entry.baseColor = std::move(newTexture);
            } else if (file.materialTextureType == MaterialTextureType::NORMAL) {
                entry.normal = std::move(newTexture);
            } else if (file.materialTextureType == MaterialTextureType::METALLIC) {
                entry.metallic = std::move(newTexture);
            } else if (file.materialTextureType == MaterialTextureType::ROUGHNESS) {
                entry.roughness = std::move(newTexture);
            }

            onLoadComplete(
                RenderSystem::MaterialTextures{
                    entry.baseColor.get(),
                    entry.normal.get(),
                    entry.metallic.get(),
                    entry.roughness.get()
                }
            );
        }

        void TextureManager::loadArrayTexture2D(const ArrayTextureFiles &files, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete)
        {
            if (files.paths.empty()) {
                return;
            }

            auto width = 0u;
            auto height = 0u;
            auto decodedImage = std::vector<unsigned char>{};
            auto newArrayTexture = std::unique_ptr<ArrayTexture2D>{};

            auto loadArrayTextureLayer = [&](const auto &filePath, unsigned int arrayTextureIndex){
                decodedImage.clear();
                lodepng::decode(decodedImage, width, height, filePath);

                if (newArrayTexture == nullptr) {
                    newArrayTexture = std::make_unique<ArrayTexture2D>(
                        initParams.numMipmapLevels,
                        initParams.internalFormat,
                        width,
                        height,
                        files.paths.size()
                    );
                }

                newArrayTexture->bufferPixelData(
                    0, //mipMap level
                    0, 0,//width, height
                    arrayTextureIndex, //array texture index
                    width,
                    height,
                    files.pixelDataFormat,
                    GL_UNSIGNED_BYTE,
                    decodedImage.data(),
                    initParams.generateMipmaps
                );
            };

            for (unsigned int i = 0; i < files.paths.size(); ++i) {
                loadArrayTextureLayer(files.paths[i], i);
            }

            auto callbackResult = newArrayTexture.get();
            arrayTextures.insert_or_assign(stringHasher(files.alias), std::move(newArrayTexture));

            onLoadComplete(callbackResult);
        }

        RenderSystem::MaterialTextures TextureManager::get(const std::string &alias) const
        {
            auto result = textures.find(stringHasher(alias));

            if (result != textures.end()) {
                const auto &internalMaterialTexture = result->second;

                return RenderSystem::MaterialTextures{
                    internalMaterialTexture.baseColor.get(),
                    internalMaterialTexture.normal.get(),
                    internalMaterialTexture.metallic.get(),
                    internalMaterialTexture.roughness.get()
                };
            }

            return RenderSystem::MaterialTextures{};
        }

        RenderSystem::Texture *TextureManager::getArrayTexture(const std::string &alias) const
        {
            auto result = arrayTextures.find(stringHasher(alias));

            if (result != arrayTextures.end()) {
                return result->second.get();
            }

            return nullptr;
        }

        void TextureManager::remove(const std::string &alias)
        {
            textures.erase(stringHasher(alias));
        }

        void TextureManager::removeArrayTexture(const std::string &alias)
        {
            arrayTextures.erase(stringHasher(alias));
        }

        void TextureManager::removeAll()
        {
            textures.clear();
            arrayTextures.clear();
        }
    }
}
