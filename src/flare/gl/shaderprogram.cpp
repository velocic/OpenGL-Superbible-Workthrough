#include <flare/gl/shaderprogram.h>

#include <algorithm>
#include <iostream>

namespace Flare
{
    namespace GL
    {
        ShaderProgram::ShaderProgram(
            const std::vector<uint8_t> &vertexShaderSource,
            const std::vector<uint8_t> &tessellationControlShaderSource,
            const std::vector<uint8_t> &tessellationEvaluationShaderSource,
            const std::vector<uint8_t> &geometryShaderSource,
            const std::vector<uint8_t> &fragmentShaderSource
        )
        {
            if (vertexShaderSource.size() > 0) {
                shaderStages.vertexShader = compileShaderProgramFromSource(vertexShaderSource, GL_VERTEX_SHADER);
            }

            if (tessellationControlShaderSource.size() > 0) {
                shaderStages.tessellationControlShader = compileShaderProgramFromSource(tessellationControlShaderSource, GL_TESS_CONTROL_SHADER);
            }

            if (tessellationEvaluationShaderSource.size() > 0) {
                shaderStages.tessellationEvaluationShader = compileShaderProgramFromSource(tessellationEvaluationShaderSource, GL_TESS_EVALUATION_SHADER);
            }

            if (geometryShaderSource.size() > 0) {
                shaderStages.geometryShader = compileShaderProgramFromSource(geometryShaderSource, GL_GEOMETRY_SHADER);
            }

            if (fragmentShaderSource.size() > 0) {
                shaderStages.fragmentShader = compileShaderProgramFromSource(fragmentShaderSource, GL_FRAGMENT_SHADER);
            }

            shaderProgram = linkShaderProgram(shaderStages);

            if (shaderProgram > 0) {
                isValid = true;
            }
        }

        ShaderProgram::ShaderProgram(ShaderProgram &&other)
        :
            shaderStages(std::move(other.shaderStages)),
            shaderProgram(other.shaderProgram),
            isValid(other.isValid),
            uniformAttributes(std::move(other.uniformAttributes)),
            diffuseTextureUnitIndices(std::move(other.diffuseTextureUnitIndices)),
            specularTextureUnitIndices(std::move(other.specularTextureUnitIndices)),
            normalTextureUnitIndices(std::move(other.normalTextureUnitIndices))
        {
            other.shaderStages = ShaderProgramStages{};
            other.shaderProgram = 0;
            other.isValid = false;
            other.uniformAttributes = std::unordered_map<std::string, GLint>();
            other.diffuseTextureUnitIndices = std::vector<unsigned int>();
            other.specularTextureUnitIndices = std::vector<unsigned int>();
            other.normalTextureUnitIndices = std::vector<unsigned int>();
        }

        ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other)
        {
            shaderStages = std::move(other.shaderStages);
            shaderProgram = other.shaderProgram;
            isValid = other.isValid;
            uniformAttributes = std::move(other.uniformAttributes);
            diffuseTextureUnitIndices = std::move(other.diffuseTextureUnitIndices);
            specularTextureUnitIndices = std::move(other.specularTextureUnitIndices);
            normalTextureUnitIndices = std::move(other.normalTextureUnitIndices);

            other.shaderStages = ShaderProgramStages{};
            other.shaderProgram = 0;
            other.isValid = false;
            other.uniformAttributes = std::unordered_map<std::string, GLint>();
            other.diffuseTextureUnitIndices = std::vector<unsigned int>();
            other.specularTextureUnitIndices = std::vector<unsigned int>();
            other.normalTextureUnitIndices = std::vector<unsigned int>();

            return *this;
        }

        ShaderProgram::~ShaderProgram()
        {
            //Calls to glDeleteShader(0) and glDeleteProgram(0) are silently ignored, according to OGL 4.5 spec
            glDeleteShader(shaderStages.vertexShader);
            glDeleteShader(shaderStages.tessellationControlShader);
            glDeleteShader(shaderStages.tessellationEvaluationShader);
            glDeleteShader(shaderStages.geometryShader);
            glDeleteShader(shaderStages.fragmentShader);

            glDeleteProgram(shaderProgram);

            isValid = false;
        }

        bool ShaderProgram::addUniformAttribute(const std::string &uniformName)
        {
            if (isShaderProgramValid() == false) {
                std::cout << "addUniformAttribute called on ShaderProgram instance with invalid state.";
                return false;
            }

            GLint uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());

            if (uniformLocation == -1) {
                return false;
            }

            uniformAttributes.insert(
                std::pair<std::string, GLint>(uniformName, uniformLocation)
            );

            return true;
        }

        void ShaderProgram::bind()
        {
            if (isShaderProgramValid() == false) {
                std::cout << "bind called on ShaderProgram instance with invalid state." << std::endl;
                return;
            }

            //Bind all necessary resources used by this material; Shader program,
            //textures & maps once available
            glUseProgram(shaderProgram);
        }

        GLint ShaderProgram::getAttribute(const std::string &attributeName) const
        {
            return glGetAttribLocation(shaderProgram, attributeName.c_str());
        }

        GLint ShaderProgram::getUniformAttribute(const std::string &uniformName)
        {
            auto mapIterator = uniformAttributes.find(uniformName);

            if (mapIterator == uniformAttributes.end()) {
                return -1;
            }

            return mapIterator->second;
        }

        bool ShaderProgram::setTextureUnits(unsigned int numDiffuseTextures, unsigned int numSpecularTextures, unsigned int numNormalTextures)
        {
            unsigned int textureUnitIndex = 0;

            for (unsigned int i = 0; i < numDiffuseTextures; ++i) {
                auto uniformName = "diffuse[" + std::to_string(i) + "]";
                auto isValidUniform = addUniformAttribute(uniformName);

                if (isValidUniform == false) {
                    std::cout << uniformName << " is an invalid uniform name" << std::endl;
                    return false;
                }

                auto texLocation = getUniformAttribute(uniformName);
                glUniform1i(texLocation, textureUnitIndex); 
                diffuseTextureUnitIndices.push_back(textureUnitIndex);
                ++textureUnitIndex;
            }

            for (unsigned int i = 0; i < numSpecularTextures; ++i) {
                auto uniformName = "specular[" + std::to_string(i) + "]";
                auto isValidUniform = addUniformAttribute(uniformName);

                if (isValidUniform == false) {
                    std::cout << uniformName << " is an invalid uniform name" << std::endl;
                    return false;
                }

                glUniform1i(getUniformAttribute(uniformName), textureUnitIndex);
                specularTextureUnitIndices.push_back(textureUnitIndex);
                ++textureUnitIndex;
            }

            for (unsigned int i = 0; i < numNormalTextures; ++i) {
                auto uniformName = "normal[" + std::to_string(i) + "]";
                auto isValidUniform = addUniformAttribute(uniformName);

                if (isValidUniform == false) {
                    std::cout << uniformName << " is an invalid uniform name" << std::endl;
                }

                glUniform1i(getUniformAttribute(uniformName), textureUnitIndex);
                normalTextureUnitIndices.push_back(textureUnitIndex);
                ++textureUnitIndex;
            }

            return true;
        }


        void ShaderProgram::unbind()
        {
            glUseProgram(0);
            glBindVertexArray(0);
        }

        GLuint ShaderProgram::compileShaderProgramFromSource(const std::vector<uint8_t> &shaderSource, GLenum shaderType)
        {
            GLuint shader = glCreateShader(shaderType);
            const GLchar *glFormatShaderSource = reinterpret_cast<const char *>(shaderSource.data());
            const GLint shaderSourceLength = shaderSource.size();
            glShaderSource(shader, 1, &glFormatShaderSource, &shaderSourceLength);
            glCompileShader(shader);

            GLint compilationSuccess = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationSuccess);

            if (!compilationSuccess) {
                GLchar infoLog[1024] = {0};
                glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);

                for (int i = 0; i < 1024; ++i) {
                    std::cout << infoLog[i];
                }
                std::cout << std::endl;

                return 0;
            }

            return shader;
        }

        GLuint ShaderProgram::linkShaderProgram(const ShaderProgramStages& shaderStages)
        {
            GLuint program = glCreateProgram();
            bool vertexShaderAttached = false;
            bool tessellationControlShaderAttached = false;
            bool tessellationEvaluationShaderAttached = false;
            bool geometryShaderAttached = false;
            bool fragmentShaderAttached = false;

            if (shaderStages.vertexShader != 0) {
                glAttachShader(program, shaderStages.vertexShader);
                vertexShaderAttached = true;
            }

            if (shaderStages.tessellationControlShader != 0) {
                glAttachShader(program, shaderStages.tessellationControlShader);
                tessellationControlShaderAttached = true;
            }

            if (shaderStages.tessellationEvaluationShader != 0) {
                glAttachShader(program, shaderStages.tessellationEvaluationShader);
                tessellationEvaluationShaderAttached = true;
            }

            if (shaderStages.geometryShader != 0) {
                glAttachShader(program, shaderStages.geometryShader);
                geometryShaderAttached = true;
            }

            if (shaderStages.fragmentShader != 0) {
                glAttachShader(program, shaderStages.fragmentShader);
                fragmentShaderAttached = true;
            }

            glLinkProgram(program);

            if (vertexShaderAttached) {
                glDetachShader(program, shaderStages.vertexShader);
            }

            if (tessellationControlShaderAttached) {
                glDetachShader(program, shaderStages.tessellationControlShader);
            }

            if (tessellationEvaluationShaderAttached) {
                glDetachShader(program, shaderStages.tessellationEvaluationShader);
            }

            if (geometryShaderAttached) {
                glDetachShader(program, shaderStages.geometryShader);
            }

            if (fragmentShaderAttached) {
                glDetachShader(program, shaderStages.fragmentShader);
            }

            GLint linkSuccess = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
            if (!linkSuccess) {
                GLchar infoLog[1024] = {0};
                glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);

                for (int i = 0; i < 1024; ++i) {
                    std::cout << infoLog[i];
                }
                std::cout << std::endl;

                return 0;
            }

            return program;
        }
    }
}
