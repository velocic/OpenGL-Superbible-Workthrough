#ifndef FLARE_GL_SHADERPROGRAM_H
#define FLARE_GL_SHADERPROGRAM_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <GL/gl3w.h>

#include <flare/gl/sampler.h>
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
                    TextureUnit(Sampler &&sampler, RenderSystem::Texture *texture, unsigned int index);
                    ~TextureUnit();
                    TextureUnit(TextureUnit &&other);
                    TextureUnit &operator=(TextureUnit &&other);
                    TextureUnit(const TextureUnit& other) = delete;
                    TextureUnit &operator=(const TextureUnit &other) = delete;

                    Sampler sampler;
                    RenderSystem::Texture *texture;
                    unsigned int index = 0;
                };

                struct TextureUnitArray {
                    TextureUnitArray(Sampler &&sampler, std::vector<RenderSystem::Texture *> &&textures, unsigned int firstIndexInclusive, unsigned int lastIndexExclusive);
                    ~TextureUnitArray();
                    TextureUnitArray(TextureUnitArray &&other);
                    TextureUnitArray &operator=(TextureUnitArray &&other);
                    TextureUnitArray(const TextureUnitArray& other) = delete;
                    TextureUnitArray &operator=(const TextureUnitArray &other) = delete;

                    Sampler sampler;
                    std::vector<RenderSystem::Texture *> textures;
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

                GLuint compileShaderProgramFromSource(const ShaderSourceFile &shaderSourceFile, GLenum shaderType);
                GLuint linkShaderProgram(const ShaderProgramStages& shaderStages);
                
                void bindTextureUnits();

                //Creates one texture unit index paired with one sampler
                void setTextureUnits(std::vector<Sampler> &&textureUnitSamplers);

                //Creates a range of texture init indices all sharing a single sampler, which will bind to an array
                //of texture samplers in glsl
                void setTextureUnitArrays(std::vector<std::pair<Sampler, unsigned int>> &&textureUnitArraySamplers);
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
                virtual GLint getUniformAttribute(const std::string &uniformName) override;
                virtual inline bool isShaderProgramValid() const override {return isValid;}
                virtual bool setTexture(const std::string &textureUnitName, RenderSystem::Texture *texture) override;
                virtual bool setTextureArrayElement(const std::string &textureUnitArrayName, unsigned int index, RenderSystem::Texture *texture) override;
                virtual bool setTextureArray(const std::string &textureUnitArrayName, std::vector<RenderSystem::Texture *> textures) override;
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

                std::vector<Sampler> textureUnitSamplers;
                std::vector<std::pair<Sampler, unsigned int>> textureUnitArraySamplers;
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

                virtual ShaderProgramBuilder& addTextureUnit(Sampler &&sampler) override
                {
                    this->textureUnitSamplers.emplace_back(std::move(sampler));
                    return *this;
                }

                virtual ShaderProgramBuilder& addTextureUnitArray(Sampler &&sampler, unsigned int numTextureUnits) override
                {
                    this->textureUnitArraySamplers.emplace_back(std::move(sampler), numTextureUnits);
                    return *this;
                }

                virtual std::unique_ptr<ShaderProgram> build() override
                {
                    auto shader = std::make_unique<Flare::GL::ShaderProgram>(
                        vertexShaderSource,
                        tessellationControlShaderSource,
                        tessellationEvaluationShaderSource,
                        geometryShaderSource,
                        fragmentShaderSource
                    );

                    shader->setTextureUnits(std::move(textureUnitSamplers));
                    shader->setTextureUnitArrays(std::move(textureUnitArraySamplers));

                    return shader;
                }
        };
    }
}

#endif
