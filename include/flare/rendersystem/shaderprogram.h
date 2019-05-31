#ifndef FLARE_RENDERSYSTEM_SHADERPROGRAM_H
#define FLARE_RENDERSYSTEM_SHADERPROGRAM_H

#include <string>

#include <flare/rendersystem/datatypes.h>

namespace Flare
{
    namespace RenderSystem
    {
        class ShaderProgram
        {
            public:
                virtual bool addUniformAttribute(const std::string &uniformName) = 0;
                virtual void bind() = 0;
                virtual RSint getAttribute(const std::string &attributeName) const = 0;
                virtual RSint getUniformAttribute(const std::string &uniformName) = 0;
                virtual bool isShaderProgramValid() const = 0;
                virtual bool setTexture(const std::string &textureUnitName, RenderSystem::Texture *texture) = 0;
                virtual bool setTextureArrayElement(const std::string &textureUnitArrayName, unsigned int index, RenderSystem::Texture *texture) = 0;
                virtual bool setTextureArray(const std::string &textureUnitArrayName, std::vector<RenderSystem::Texture *> textures) = 0;
                virtual void unbind() = 0;
        };
    }
}

#endif
