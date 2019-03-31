#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <GL/gl3w.h>

namespace Flare
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
            ShaderProgramStages shaderStages;
            GLuint shaderProgram = 0;
            bool isValid = false;
            //Map of the variables we've enabled in the shader using glEnableVertexAttribArray after querying
            //an attribute index with glGetAttribLocation, sorted per vertex array object
            std::unordered_map<GLuint, std::vector<std::pair<std::string, GLint>>> shaderAttributes;
            std::unordered_map<std::string, GLint> uniformAttributes;

            std::vector<unsigned int> diffuseTextureUnitIndices;
            std::vector<unsigned int> specularTextureUnitIndices;
            std::vector<unsigned int> normalTextureUnitIndices;

            GLuint compileShaderProgramFromSource(const std::vector<uint8_t> &shaderSource, GLenum shaderType);
            GLuint linkShaderProgram(const ShaderProgramStages& shaderStages);
        public:
            ShaderProgram(
                const std::vector<uint8_t> &vertexShaderSource,
                const std::vector<uint8_t> &tessellationControlShaderSource,
                const std::vector<uint8_t> &tessellationEvaluationShaderSource,
                const std::vector<uint8_t> &geometryShaderSource,
                const std::vector<uint8_t> &fragmentShaderSource
            );

            //Delete shaders, linked program
            ~ShaderProgram();

            bool addUniformAttribute(const std::string &uniformName);
            void bind();
            GLint getAttribute(const std::string &attributeName) const;
            GLint getUniformAttribute(const std::string &uniformName);
            const std::vector<unsigned int> &getDiffuseTextureUnits() const {return diffuseTextureUnitIndices;}
            const std::vector<unsigned int> &getSpecularTextureUnits() const {return specularTextureUnitIndices;}
            const std::vector<unsigned int> &getNormalTextureUnits() const {return normalTextureUnitIndices;}
            inline bool isShaderProgramValid() const {return isValid;}
            bool setTextureUnits(unsigned int numDiffuseTextures, unsigned int numSpecularTextures, unsigned int numNormalTextures);

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

            virtual std::unique_ptr<ShaderProgram> build()
            {
                return std::make_unique<Flare::ShaderProgram>(
                    vertexShaderSource,
                    tessellationControlShaderSource,
                    tessellationEvaluationShaderSource,
                    geometryShaderSource,
                    fragmentShaderSource
                );
            }
    };
}

#endif
