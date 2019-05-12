#ifndef FLARE_GL_SHADERPROGRAM_H
#define FLARE_GL_SHADERPROGRAM_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <GL/gl3w.h>

#include <flare/gl/sampler.h>
#include <flare/gl/texture.h>

namespace Flare
{
    namespace GL
    {
        struct ShaderProgramStages
        {
            GLuint vertexShader = 0;
            GLuint tessellationControlShader = 0;
            GLuint tessellationEvaluationShader = 0;
            GLuint geometryShader = 0;
            GLuint fragmentShader = 0;
        };

        class ShaderProgram
        {
            private:
                struct TextureUnit {
                    TextureUnit(Sampler &&sampler, std::shared_ptr<Texture> texture, unsigned int index)
                    :
                        sampler(std::move(sampler)), texture(texture), index(index)
                    {}
                    ~TextureUnit() {}
                    TextureUnit(TextureUnit &&other)
                    :
                        sampler(std::move(other.sampler)), texture(std::move(other.texture)), index(other.index)
                    {
                        other.index = 0;
                    }
                    TextureUnit &operator=(TextureUnit &&other)
                    {
                        sampler = std::move(other.sampler);
                        texture = std::move(other.texture);
                        index = other.index;

                        other.index = 0;

                        return *this;
                    }
                    TextureUnit(const TextureUnit& other) = delete;
                    TextureUnit &operator=(const TextureUnit &other) = delete;

                    Sampler sampler;
                    std::shared_ptr<Texture> texture;
                    unsigned int index = 0;
                };

                struct TextureUnitArray {
                    TextureUnitArray(Sampler &&sampler, std::vector<std::shared_ptr<Texture>> &&textures, unsigned int firstIndexInclusive, unsigned int lastIndexExclusive)
                    :
                        sampler(std::move(sampler)), textures(std::move(textures)), firstIndexInclusive(firstIndexInclusive), lastIndexExclusive(lastIndexExclusive)
                    {}
                    ~TextureUnitArray() {}
                    TextureUnitArray(TextureUnitArray &&other)
                    :
                        sampler(std::move(other.sampler)), textures(std::move(other.textures)), firstIndexInclusive(other.firstIndexInclusive), lastIndexExclusive(other.lastIndexExclusive)
                    {
                        other.firstIndexInclusive = 0;
                        other.lastIndexExclusive = 0;
                    }
                    TextureUnitArray &operator=(TextureUnitArray &&other)
                    {
                        sampler = std::move(other.sampler);
                        textures = std::move(other.textures);
                        firstIndexInclusive = other.firstIndexInclusive;
                        lastIndexExclusive = other.lastIndexExclusive;

                        other.firstIndexInclusive = 0;
                        other.lastIndexExclusive = 0;

                        return *this;
                    }
                    TextureUnitArray(const TextureUnitArray& other) = delete;
                    TextureUnitArray &operator=(const TextureUnitArray &other) = delete;

                    Sampler sampler;
                    std::vector<std::shared_ptr<Texture>> textures;
                    unsigned int firstIndexInclusive = 0;
                    unsigned int lastIndexExclusive = 0;
                };

                ShaderProgramStages shaderStages;
                GLuint shaderProgram = 0;
                std::unordered_map<std::string, GLint> uniformAttributes;
                std::unordered_map<std::string, TextureUnit> textureUnits;
                std::unordered_map<std::string, TextureUnitArray> textureUnitArrays;
                unsigned int totalAssignedTextureUnits = 0;
                bool isValid = false;

                GLuint compileShaderProgramFromSource(const std::vector<uint8_t> &shaderSource, GLenum shaderType);
                GLuint linkShaderProgram(const ShaderProgramStages& shaderStages);
                
                void bindTextureUnits();

                //Creates one texture unit index paired with one sampler
                void setTextureUnits(std::vector<Sampler> &&textureUnitSamplers);

                //Creates a range of texture init indices all sharing a single sampler, which will bind to an array
                //of texture samplers in glsl
                void setTextureUnitArrays(std::vector<std::pair<Sampler, unsigned int>> &&textureUnitArraySamplers);
        public:
                ShaderProgram(
                    const std::vector<uint8_t> &vertexShaderSource,
                    const std::vector<uint8_t> &tessellationControlShaderSource,
                    const std::vector<uint8_t> &tessellationEvaluationShaderSource,
                    const std::vector<uint8_t> &geometryShaderSource,
                    const std::vector<uint8_t> &fragmentShaderSource
                );
                ShaderProgram(ShaderProgram &&other);
                ShaderProgram &operator=(ShaderProgram &&other);
                ShaderProgram(const ShaderProgram &other) = delete;
                ShaderProgram &operator=(const ShaderProgram &other) = delete;

                //Delete shaders, linked program
                ~ShaderProgram();

                bool addUniformAttribute(const std::string &uniformName);
                void bind();
                GLint getAttribute(const std::string &attributeName) const;
                GLint getUniformAttribute(const std::string &uniformName);
                inline bool isShaderProgramValid() const {return isValid;}

                void unbind();
        };

        class ShaderProgramBuilder
        {
            private:
                std::vector<uint8_t> vertexShaderSource;
                std::vector<uint8_t> tessellationControlShaderSource;
                std::vector<uint8_t> tessellationEvaluationShaderSource;
                std::vector<uint8_t> geometryShaderSource;
                std::vector<uint8_t> fragmentShaderSource;
                std::vector<Sampler> textureUnitSamplers;
                std::vector<std::pair<Sampler, unsigned int>> textureUnitArraySamplers;
            public:
                virtual ShaderProgramBuilder& addVertexShader(std::vector<uint8_t>&& vertexShaderSource)
                {
                    this->vertexShaderSource = vertexShaderSource;
                    return *this;
                }

                virtual ShaderProgramBuilder& addTessellationControlShader(std::vector<uint8_t>&& tessellationControlShaderSource)
                {
                    this->tessellationControlShaderSource = tessellationControlShaderSource;
                    return *this;
                }

                virtual ShaderProgramBuilder& addTessellationEvaluationShader(std::vector<uint8_t>&& tessellationEvaluationShaderSource)
                {
                    this->tessellationEvaluationShaderSource = tessellationEvaluationShaderSource;
                    return *this;
                }

                virtual ShaderProgramBuilder& addGeometryShader(std::vector<uint8_t>&& geometryShaderSource)
                {
                    this->geometryShaderSource = geometryShaderSource;
                    return *this;
                }

                virtual ShaderProgramBuilder& addFragmentShader(std::vector<uint8_t>&& fragmentShaderSource)
                {
                    this->fragmentShaderSource = fragmentShaderSource;
                    return *this;
                }

                virtual ShaderProgramBuilder& addTextureUnit(Sampler &&sampler)
                {
                    this->textureUnitSamplers.emplace_back(std::move(sampler));
                    return *this;
                }

                virtual ShaderProgramBuilder& addTextureUnitArray(Sampler &&sampler, unsigned int numTextureUnits)
                {
                    this->textureUnitArraySamplers.emplace_back(std::move(sampler), numTextureUnits);
                    return *this;
                }

                virtual std::unique_ptr<ShaderProgram> build()
                {
                    return std::make_unique<Flare::GL::ShaderProgram>(
                        vertexShaderSource,
                        tessellationControlShaderSource,
                        tessellationEvaluationShaderSource,
                        geometryShaderSource,
                        fragmentShaderSource
                    );
                }
        };
    }
}

#endif
