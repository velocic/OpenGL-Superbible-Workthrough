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
            PBRTextures(std::move(other.PBRTextures)),
            phongTextures(std::move(other.phongTextures)),
            arrayTextures(std::move(other.arrayTextures)),
            textures(std::move(other.textures))
        {}

        TextureManager &TextureManager::operator=(TextureManager &&other)
        {
            PBRTextures = std::move(other.PBRTextures);
            phongTextures = std::move(other.phongTextures);
            arrayTextures = std::move(other.arrayTextures);
            textures = std::move(other.textures);

            return *this;
        }

        void TextureManager::batchLoadTexture1D(const std::vector<PBRTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::batchLoadTexture1D() not yet implemented.");
        }

        void TextureManager::batchLoadTexture1D(const std::vector<PhongTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::batchLoadTexture1D() not yet implemented.");
        }

        void TextureManager::batchLoadTexture1D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::batchLoadTexture1D() not yet implemented.");
        }

        void TextureManager::batchLoadTexture2D(const std::vector<PBRTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            for (const auto &target : targets) {
                loadTexture2D(target, initParams, [](auto){});
            }

            onLoadComplete();
        }

        void TextureManager::batchLoadTexture2D(const std::vector<PhongTextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete) {
            for (const auto &target : targets) {
                loadTexture2D(target, initParams, [](auto){});
            }
        }

        void TextureManager::batchLoadTexture2D(const std::vector<TextureFile> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            for (const auto &target : targets) {
                loadTexture2D(target, initParams, [](auto){});
            }
        }

        void TextureManager::batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, const TextureInitParams &initParams, std::function<void()> onLoadComplete)
        {
            for (const auto &target : targets) {
                loadArrayTexture2D(target, initParams, [](auto){});
            }

            onLoadComplete();
        }

        void TextureManager::loadTexture1D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::loadTexture1D() not yet implemented.");
        }

        void TextureManager::loadTexture1D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::loadTexture1D() not yet implemented.");
        }

        void TextureManager::loadTexture1D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete)
        {
            throw std::runtime_error("GL::TextureManager::loadTexture1D() not yet implemented.");
        }

        void TextureManager::loadTexture2D(const PBRTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PBRMaterialTextures)> onLoadComplete)
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

            auto entryExists = PBRTextures.find(lookupKey) != PBRTextures.end();
            if (!entryExists) {
                auto newEntry = PBRMaterialTextures{};
                newEntry.materialId = lookupKey;
                PBRTextures.insert_or_assign(lookupKey, std::move(newEntry));
            }
            auto &entry = PBRTextures.find(lookupKey)->second;

            if (file.materialTextureType == PBRMaterialTextureType::BASE_COLOR) {
                entry.baseColor.emplace_back(std::move(newTexture));
            } else if (file.materialTextureType == PBRMaterialTextureType::NORMAL) {
                entry.normal.emplace_back(std::move(newTexture));
            } else if (file.materialTextureType == PBRMaterialTextureType::METALLIC) {
                entry.metallic.emplace_back(std::move(newTexture));
            } else if (file.materialTextureType == PBRMaterialTextureType::ROUGHNESS) {
                entry.roughness.emplace_back(std::move(newTexture));
            }

            onLoadComplete(getNonOwningMaterialPointers(entry));
        }

        void TextureManager::loadTexture2D(const PhongTextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::PhongMaterialTextures)> onLoadComplete)
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

            auto entryExists = phongTextures.find(lookupKey) != phongTextures.end();
            if (!entryExists) {
                auto newEntry = PhongMaterialTextures{};
                newEntry.materialId = lookupKey;
                phongTextures.insert_or_assign(lookupKey, std::move(newEntry));
            }
            auto &entry = phongTextures.find(lookupKey)->second;

            if (file.materialTextureType == PhongMaterialTextureType::DIFFUSE) {
                entry.diffuse.emplace_back(std::move(newTexture));
            } else if (file.materialTextureType == PhongMaterialTextureType::SPECULAR) {
                entry.specular.emplace_back(std::move(newTexture));
            } else if (file.materialTextureType == PhongMaterialTextureType::NORMAL) {
                entry.normal.emplace_back(std::move(newTexture));
            }

            onLoadComplete(getNonOwningMaterialPointers(entry));
        }

        void TextureManager::loadTexture2D(const TextureFile &file, const TextureInitParams &initParams, std::function<void(RenderSystem::Texture *)> onLoadComplete)
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

            auto result = newTexture.get();
            textures.insert_or_assign(lookupKey, std::move(newTexture));
            onLoadComplete(result);
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

        RenderSystem::Texture *TextureManager::get(const std::string &alias) const
        {
            auto result = textures.find(stringHasher(alias));

            if (result != textures.end()) {
                return result->second.get();
            }

            return nullptr;
        }

        RenderSystem::PBRMaterialTextures TextureManager::getPBRMaterialTextures(const std::string &alias) const
        {
            auto result = PBRTextures.find(stringHasher(alias));

            if (result != PBRTextures.end()) {
                return getNonOwningMaterialPointers(result->second);
            }

            return RenderSystem::PBRMaterialTextures{};
        }

        RenderSystem::PhongMaterialTextures TextureManager::getPhongMaterialTextures(const std::string &alias) const
        {
            auto result = phongTextures.find(stringHasher(alias));

            if (result != phongTextures.end()) {
                return getNonOwningMaterialPointers(result->second);
            }

            return RenderSystem::PhongMaterialTextures{};
        }

        RenderSystem::Texture *TextureManager::getArrayTexture(const std::string &alias) const
        {
            auto result = arrayTextures.find(stringHasher(alias));

            if (result != arrayTextures.end()) {
                return result->second.get();
            }

            return nullptr;
        }

        bool TextureManager::arePBRMaterialTexturesLoaded(const std::string &alias) const
        {
            return PBRTextures.find(stringHasher(alias)) != PBRTextures.end();
        }

        bool TextureManager::arePhongMaterialTexturesLoaded(const std::string &alias) const
        {
            return phongTextures.find(stringHasher(alias)) != phongTextures.end();
        }

        void TextureManager::remove(const std::string &alias)
        {
            textures.erase(stringHasher(alias));
        }

        void TextureManager::removePBRMaterialTextures(const std::string &alias)
        {
            PBRTextures.erase(stringHasher(alias));
        }

        void TextureManager::removePhongMaterialTextures(const std::string &alias)
        {
            phongTextures.erase(stringHasher(alias));
        }

        void TextureManager::removeArrayTexture(const std::string &alias)
        {
            arrayTextures.erase(stringHasher(alias));
        }

        void TextureManager::removeAll()
        {
            PBRTextures.clear();
            phongTextures.clear();
            arrayTextures.clear();
            textures.clear();
        }

        RenderSystem::PBRMaterialTextures TextureManager::getNonOwningMaterialPointers(const PBRMaterialTextures &owningPointers) const
        {
            auto nonOwningPointers = RenderSystem::PBRMaterialTextures{};

            nonOwningPointers.baseColor.reserve(owningPointers.baseColor.size());
            nonOwningPointers.normal.reserve(owningPointers.normal.size());
            nonOwningPointers.metallic.reserve(owningPointers.metallic.size());
            nonOwningPointers.roughness.reserve(owningPointers.roughness.size());

            for (const auto &texture : owningPointers.baseColor) {
                nonOwningPointers.baseColor.push_back(texture.get());
            }

            for (const auto &texture : owningPointers.normal) {
                nonOwningPointers.normal.push_back(texture.get());
            }

            for (const auto &texture : owningPointers.metallic) {
                nonOwningPointers.metallic.push_back(texture.get());
            }

            for (const auto &texture : owningPointers.roughness) {
                nonOwningPointers.roughness.push_back(texture.get());
            }

            nonOwningPointers.materialId = owningPointers.materialId;

            return nonOwningPointers;
        }

        RenderSystem::PhongMaterialTextures TextureManager::getNonOwningMaterialPointers(const PhongMaterialTextures &owningPointers) const
        {
            auto nonOwningPointers = RenderSystem::PhongMaterialTextures{};

            nonOwningPointers.diffuse.reserve(owningPointers.diffuse.size());
            nonOwningPointers.specular.reserve(owningPointers.specular.size());
            nonOwningPointers.normal.reserve(owningPointers.normal.size());

            for (const auto &texture : owningPointers.diffuse) {
                nonOwningPointers.diffuse.push_back(texture.get());
            }

            for (const auto &texture : owningPointers.specular) {
                nonOwningPointers.specular.push_back(texture.get());
            }

            for (const auto &texture : owningPointers.normal) {
                nonOwningPointers.normal.push_back(texture.get());
            }

            nonOwningPointers.materialId = owningPointers.materialId;

            return nonOwningPointers;
        }
    }
}
