#ifndef FLARE_GL_SHADERPROGRAM_H
#define FLARE_GL_SHADERPROGRAM_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <GL/gl3w.h>

#include <flare/gl/transformfeedbackbuffermanager.h>
#include <flare/rendersystem/sampler.h>
#include <flare/rendersystem/texture.h>
#include <flare/rendersystem/shaderprogram.h>
#include <flare/utility/file.h>

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

        struct ShaderSourceFile
        {
            std::string filePath;
            std::vector<uint8_t> sourceCode;
        };

        class ShaderProgram : public RenderSystem::ShaderProgram
        {
            friend class ShaderProgramBuilder;

            private:
                struct TextureUnit {
                    TextureUnit(RenderSystem::Sampler *sampler, RenderSystem::Texture *texture, unsigned int index);
                    ~TextureUnit();
                    TextureUnit(TextureUnit &&other);
                    TextureUnit &operator=(TextureUnit &&other);
                    TextureUnit(const TextureUnit& other) = delete;
                    TextureUnit &operator=(const TextureUnit &other) = delete;

                    RenderSystem::Sampler *sampler = nullptr;
                    RenderSystem::Texture *texture = nullptr;
                    unsigned int index = 0;
                };

                struct TextureUnitArray {
                    TextureUnitArray(RenderSystem::Sampler *sampler, std::vector<RenderSystem::Texture *> &&textures, unsigned int firstIndexInclusive, unsigned int lastIndexExclusive);
                    ~TextureUnitArray();
                    TextureUnitArray(TextureUnitArray &&other);
                    TextureUnitArray &operator=(TextureUnitArray &&other);
                    TextureUnitArray(const TextureUnitArray& other) = delete;
                    TextureUnitArray &operator=(const TextureUnitArray &other) = delete;

                    RenderSystem::Sampler *sampler;
                    std::vector<RenderSystem::Texture *> textures;
                    unsigned int firstIndexInclusive = 0;
                    unsigned int lastIndexExclusive = 0;
                };

                ShaderProgramStages shaderStages;
                GLuint shaderProgram = 0;
                std::unordered_map<std::string, GLint> uniformAttributes;
                std::unordered_map<size_t, TextureUnit> textureUnits;
                std::unordered_map<size_t, TextureUnitArray> textureUnitArrays;
                unsigned int totalAssignedTextureUnits = 0;
                bool isValid = false;

                GLuint compileShaderProgramFromSource(const ShaderSourceFile &shaderSourceFile, GLenum shaderType);
                GLuint linkShaderProgram(const ShaderProgramStages& shaderStages);
                
                void bindTextureUnits();

                //Creates one texture unit index paired with one sampler
                void setTextureUnits(const std::vector<RenderSystem::Sampler *> &textureUnitSamplers);

                //Creates a range of texture init indices all sharing a single sampler, which will bind to an array
                //of texture samplers in glsl
                void setTextureUnitArrays(const std::vector<std::pair<RenderSystem::Sampler *, unsigned int>> &textureUnitArraySamplers);
            public:
                ShaderProgram(
                    const ShaderSourceFile &vertexShaderSource,
                    const ShaderSourceFile &tessellationControlShaderSource,
                    const ShaderSourceFile &tessellationEvaluationShaderSource,
                    const ShaderSourceFile &geometryShaderSource,
                    const ShaderSourceFile &fragmentShaderSource
                );
                ShaderProgram(ShaderProgram &&other);
                ShaderProgram &operator=(ShaderProgram &&other);
                ShaderProgram(const ShaderProgram &other) = delete;
                ShaderProgram &operator=(const ShaderProgram &other) = delete;

                //Delete shaders, linked program
                virtual ~ShaderProgram();

                virtual bool addUniformAttribute(const std::string &uniformName) override;
                virtual void bind() override;
                virtual GLint getAttribute(const std::string &attributeName) const override;
                virtual GLuint getProgramId() const override;
                virtual GLint getUniformAttribute(const std::string &uniformName) override;
                virtual bool isShaderProgramValid() const override {return isValid;}
                virtual bool setTexture(size_t textureUnitName, RenderSystem::Texture *texture) override;
                virtual bool setTextureArrayElement(size_t textureUnitArrayName, unsigned int index, RenderSystem::Texture *texture) override;
                virtual bool setTextureArray(size_t textureUnitArrayName, const std::vector<RenderSystem::Texture *> &textures) override;
                virtual bool setTexture(const std::string &textureUnitName, RenderSystem::Texture *texture) override;
                virtual bool setTextureArrayElement(const std::string &textureUnitArrayName, unsigned int index, RenderSystem::Texture *texture) override;
                virtual bool setTextureArray(const std::string &textureUnitArrayName, const std::vector<RenderSystem::Texture *> &textures) override;
                virtual void unbind() override;
        };

        class ShaderProgramBuilder : public RenderSystem::ShaderProgramBuilder
        {
            private:
                ShaderSourceFile vertexShaderSource;
                ShaderSourceFile tessellationControlShaderSource;
                ShaderSourceFile tessellationEvaluationShaderSource;
                ShaderSourceFile geometryShaderSource;
                ShaderSourceFile fragmentShaderSource;

                std::vector<RenderSystem::Sampler *> textureUnitSamplers;
                std::vector<std::pair<RenderSystem::Sampler *, unsigned int>> textureUnitArraySamplers;
            public:
                virtual ShaderProgramBuilder& setVertexShader(const std::string &vertexShaderFilePath) override
                {
                    vertexShaderSource.filePath = vertexShaderFilePath;
                    vertexShaderSource.sourceCode = ::Flare::File::getFileContents(vertexShaderFilePath);

                    return *this;
                }

                virtual ShaderProgramBuilder& setTessellationControlShader(const std::string &tessellationControlShaderFilePath) override
                {
                    tessellationControlShaderSource.filePath = tessellationControlShaderFilePath;
                    tessellationControlShaderSource.sourceCode = ::Flare::File::getFileContents(tessellationControlShaderFilePath);

                    return *this;
                }

                virtual ShaderProgramBuilder& setTessellationEvaluationShader(const std::string &tessellationEvaluationShaderFilePath) override
                {
                    tessellationEvaluationShaderSource.filePath = tessellationEvaluationShaderFilePath;
                    tessellationEvaluationShaderSource.sourceCode = ::Flare::File::getFileContents(tessellationEvaluationShaderFilePath);

                    return *this;
                }

                virtual ShaderProgramBuilder& setGeometryShader(const std::string &geometryShaderFilePath) override
                {
                    geometryShaderSource.filePath = geometryShaderFilePath;
                    geometryShaderSource.sourceCode = ::Flare::File::getFileContents(geometryShaderFilePath);

                    return *this;
                }

                virtual ShaderProgramBuilder& setFragmentShader(const std::string &fragmentShaderFilePath) override
                {
                    fragmentShaderSource.filePath = fragmentShaderFilePath;
                    fragmentShaderSource.sourceCode = ::Flare::File::getFileContents(fragmentShaderFilePath);

                    return *this;
                }

                virtual ShaderProgramBuilder& addTextureUnit(RenderSystem::Sampler *sampler) override
                {
                    this->textureUnitSamplers.push_back(sampler);
                    return *this;
                }

                virtual ShaderProgramBuilder& addTextureUnitArray(RenderSystem::Sampler *sampler, unsigned int numTextureUnits) override
                {
                    this->textureUnitArraySamplers.emplace_back(sampler, numTextureUnits);
                    return *this;
                }

                virtual std::unique_ptr<RenderSystem::ShaderProgram> build() override
                {
                    auto shader = std::make_unique<Flare::GL::ShaderProgram>(
                        vertexShaderSource,
                        tessellationControlShaderSource,
                        tessellationEvaluationShaderSource,
                        geometryShaderSource,
                        fragmentShaderSource
                    );

                    shader->setTextureUnits(textureUnitSamplers);
                    shader->setTextureUnitArrays(textureUnitArraySamplers);

                    return shader;
                }
        };
    }
}

#endif
