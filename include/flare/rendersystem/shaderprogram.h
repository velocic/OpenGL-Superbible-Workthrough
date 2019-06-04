#ifndef FLARE_RENDERSYSTEM_SHADERPROGRAM_H
#define FLARE_RENDERSYSTEM_SHADERPROGRAM_H

#include <memory>
#include <string>

#include <flare/rendersystem/datatypes.h>
#include <flare/rendersystem/sampler.h>
#include <flare/rendersystem/texture.h>

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
                virtual bool setTexture(size_t textureUnitName, RenderSystem::Texture *texture) = 0;
                virtual bool setTextureArrayElement(size_t textureUnitArrayName, unsigned int index, RenderSystem::Texture *texture) = 0;
                virtual bool setTextureArray(size_t textureUnitArrayName, const std::vector<RenderSystem::Texture *> &textures) = 0;
                virtual bool setTexture(const std::string &textureUnitName, RenderSystem::Texture *texture) = 0;
                virtual bool setTextureArrayElement(const std::string &textureUnitArrayName, unsigned int index, RenderSystem::Texture *texture) = 0;
                virtual bool setTextureArray(const std::string &textureUnitArrayName, const std::vector<RenderSystem::Texture *> &textures) = 0;
                virtual void unbind() = 0;
        };

        class ShaderProgramBuilder
        {
            public:
                virtual ShaderProgramBuilder& setVertexShader(const std::string &vertexShaderFilePath) = 0;
                virtual ShaderProgramBuilder& setTessellationControlShader(const std::string &tessellationControlShaderFilePath) = 0;
                virtual ShaderProgramBuilder& setTessellationEvaluationShader(const std::string &tessellationEvaluationShaderFilePath) = 0;
                virtual ShaderProgramBuilder& setGeometryShader(const std::string &geometryShaderFilePath) = 0;
                virtual ShaderProgramBuilder& setFragmentShader(const std::string &fragmentShaderFilePath) = 0;
                virtual ShaderProgramBuilder& addTextureUnit(Sampler *sampler) = 0;
                virtual ShaderProgramBuilder& addTextureUnitArray(Sampler *sampler, unsigned int numTextureUnits) = 0;
                virtual std::unique_ptr<ShaderProgram> build() = 0;
        };
    }
}

#endif
