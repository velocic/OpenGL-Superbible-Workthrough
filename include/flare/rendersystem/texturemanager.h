#ifndef FLARE_RENDERSYSTEM_TEXTUREMANAGER_H
#define FLARE_RENDERSYSTEM_TEXTUREMANAGER_H

#include <functional>
#include <string>

#include <flare/rendersystem/texture.h>

namespace Flare
{
    namespace RenderSystem
    {
        class TextureManager
        {
            public:
                enum class SupportedFileType {
                    PNG
                };

                struct TextureFile {
                    std::string path;
                    std::string alias;
                    SupportedFileType type;
                };

                struct ArrayTextureFiles {
                    std::vector<std::string> paths;
                    std::string alias;
                    SupportedFileType type;
                };

                virtual void batchLoadTexture1D(const std::vector<TextureFile> &targets, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadTexture2D(const std::vector<TextureFile> &targets, std::function<void()> onLoadComplete) = 0;
                virtual void batchLoadArrayTexture2D(const std::vector<ArrayTextureFiles> &targets, std::function<void()> onLoadComplete) = 0;

                virtual void loadTexture1D(const TextureFile &file, std::function<void(Texture *)> onLoadComplete) = 0;
                virtual void loadTexture2D(const TextureFile &file, std::function<void(Texture *)> onLoadComplete) = 0;
                virtual void loadArrayTexture2D(const ArrayTextureFiles &files, std::function<void(Texture *)> onLoadComplete) = 0;

                //TODO: support loading in-memory textures (for making runtime-generated textures available)

                virtual Texture *get(const std::string &alias) = 0;
                virtual void remove(const std::string &alias) = 0;
                virtual void removeAll() = 0;
        };
    }
}

#endif
