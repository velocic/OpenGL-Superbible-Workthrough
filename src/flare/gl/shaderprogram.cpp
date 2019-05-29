#include <flare/gl/shaderprogram.h>

#include <algorithm>
#include <iostream>

namespace Flare
{
    namespace GL
    {
        ShaderProgram::ShaderProgram(
            const ShaderSourceFile &vertexShaderSourceFile,
            const ShaderSourceFile &tessellationControlShaderSourceFile,
            const ShaderSourceFile &tessellationEvaluationShaderSourceFile,
            const ShaderSourceFile &geometryShaderSourceFile,
            const ShaderSourceFile &fragmentShaderSourceFile
        )
        {
            if (vertexShaderSourceFile.sourceCode.size() > 0) {
                shaderStages.vertexShader = compileShaderProgramFromSource(vertexShaderSourceFile, GL_VERTEX_SHADER);
            }

            if (tessellationControlShaderSourceFile.sourceCode.size() > 0) {
                shaderStages.tessellationControlShader = compileShaderProgramFromSource(tessellationControlShaderSourceFile, GL_TESS_CONTROL_SHADER);
            }

            if (tessellationEvaluationShaderSourceFile.sourceCode.size() > 0) {
                shaderStages.tessellationEvaluationShader = compileShaderProgramFromSource(tessellationEvaluationShaderSourceFile, GL_TESS_EVALUATION_SHADER);
            }

            if (geometryShaderSourceFile.sourceCode.size() > 0) {
                shaderStages.geometryShader = compileShaderProgramFromSource(geometryShaderSourceFile, GL_GEOMETRY_SHADER);
            }

            if (fragmentShaderSourceFile.sourceCode.size() > 0) {
                shaderStages.fragmentShader = compileShaderProgramFromSource(fragmentShaderSourceFile, GL_FRAGMENT_SHADER);
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
            uniformAttributes(std::move(other.uniformAttributes)),
            textureUnits(std::move(other.textureUnits)),
            textureUnitArrays(std::move(other.textureUnitArrays)),
            isValid(other.isValid)
        {
            other.shaderStages = ShaderProgramStages{};
            other.shaderProgram = 0;
            other.isValid = false;
            other.uniformAttributes = std::unordered_map<std::string, GLint>{};
            other.textureUnits = std::unordered_map<std::string, TextureUnit>{};
            other.textureUnitArrays = std::unordered_map<std::string, TextureUnitArray>{};
        }

        ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other)
        {
            shaderStages = std::move(other.shaderStages);
            shaderProgram = other.shaderProgram;
            uniformAttributes = std::move(other.uniformAttributes);
            textureUnits = std::move(other.textureUnits);
            textureUnitArrays = std::move(other.textureUnitArrays);
            isValid = other.isValid;

            other.shaderStages = ShaderProgramStages{};
            other.shaderProgram = 0;
            other.isValid = false;
            other.uniformAttributes = std::unordered_map<std::string, GLint>{};
            other.textureUnits = std::unordered_map<std::string, TextureUnit>{};
            other.textureUnitArrays = std::unordered_map<std::string, TextureUnitArray>{};

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

            glUseProgram(shaderProgram);

            bindTextureUnits();
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

        void ShaderProgram::unbind()
        {
            glUseProgram(0);
            glBindVertexArray(0);

            //TODO: unbind all textures/samplers?
        }

        GLuint ShaderProgram::compileShaderProgramFromSource(const ShaderSourceFile &shaderSourceFile, GLenum shaderType)
        {
            GLuint shader = glCreateShader(shaderType);
            const GLchar *glFormatShaderSource = reinterpret_cast<const char *>(shaderSourceFile.sourceCode.data());
            const GLint shaderSourceLength = shaderSourceFile.sourceCode.size();
            glShaderSource(shader, 1, &glFormatShaderSource, &shaderSourceLength);
            glCompileShader(shader);

            GLint compilationSuccess = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationSuccess);

            if (!compilationSuccess) {
                GLint infoLogLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

                std::vector<GLchar> infoLog;
                infoLog.resize(infoLogLength);

                glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());

                std::cout << "Shader compilation error(s) in " << shaderSourceFile.filePath << ":" << std::endl;
                for (int i = 0; i < infoLogLength; ++i) {
                    std::cout << infoLog[i];
                }
                std::cout << std::endl;

                return 0;
            }

            return shader;
        }

        void ShaderProgram::bindTextureUnits()
        {
            for (auto &textureUnitKVPair : textureUnits) {
                auto &textureUnit = textureUnitKVPair.second;
                textureUnit.texture->bind(textureUnit.index);
                textureUnit.sampler.bind(textureUnit.index);
            }

            for (auto &textureUnitArrayKVPair : textureUnitArrays) {
                auto &textureUnitArray = textureUnitArrayKVPair.second;
                unsigned int currentTextureOffset = 0;

                for (auto &texture : textureUnitArray.textures) {
                    auto textureUnitIndex = textureUnitArray.firstIndexInclusive + currentTextureOffset;
                    texture->bind(textureUnitIndex);
                    textureUnitArray.sampler.bind(textureUnitIndex);
                }
            }
        }

        void ShaderProgram::setTextureUnits(std::vector<Sampler> &&textureUnitSamplers)
        {
            for (auto &&sampler : textureUnitSamplers) {
                auto samplerName = std::string(sampler.getName());

                textureUnits.insert_or_assign(
                    samplerName,
                    TextureUnit(std::move(sampler), nullptr, totalAssignedTextureUnits++)
                );
            }

            textureUnitSamplers = std::vector<Sampler>{};
        }

        void ShaderProgram::setTextureUnitArrays(std::vector<std::pair<Sampler, unsigned int>> &&textureUnitArraySamplers)
        {
            for (auto &&samplerArrayEntry : textureUnitArraySamplers) {
                const auto &numArrayElements = samplerArrayEntry.second;
                auto samplerName = std::string(samplerArrayEntry.first.getName());

                textureUnitArrays.insert_or_assign(
                    samplerName,
                    TextureUnitArray(
                        std::move(samplerArrayEntry.first),
                        std::vector<RenderSystem::Texture *>(numArrayElements),
                        totalAssignedTextureUnits,
                        totalAssignedTextureUnits + numArrayElements
                    )
                );

                totalAssignedTextureUnits += numArrayElements;
            }

            textureUnitArraySamplers = std::vector<std::pair<Sampler, unsigned int>>{};
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
                glDeleteShader(shaderStages.vertexShader);
            }

            if (tessellationControlShaderAttached) {
                glDetachShader(program, shaderStages.tessellationControlShader);
                glDeleteShader(shaderStages.tessellationControlShader);
            }

            if (tessellationEvaluationShaderAttached) {
                glDetachShader(program, shaderStages.tessellationEvaluationShader);
                glDeleteShader(shaderStages.tessellationEvaluationShader);
            }

            if (geometryShaderAttached) {
                glDetachShader(program, shaderStages.geometryShader);
                glDeleteShader(shaderStages.geometryShader);
            }

            if (fragmentShaderAttached) {
                glDetachShader(program, shaderStages.fragmentShader);
                glDeleteShader(shaderStages.fragmentShader);
            }

            GLint linkSuccess = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);

            if (!linkSuccess) {
                GLint infoLogLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

                std::vector<GLchar> infoLog;
                infoLog.resize(infoLogLength);

                glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

                for (int i = 0; i < infoLogLength; ++i) {
                    std::cout << infoLog[i];
                }
                std::cout << std::endl;

                return 0;
            }

            return program;
        }

        bool ShaderProgram::setTexture(const std::string &textureUnitName, RenderSystem::Texture *texture)
        {
            auto mapIterator = textureUnits.find(textureUnitName);

            if (mapIterator == textureUnits.end()) {
                return false;
            }

            mapIterator->second.texture = texture;

            return true;
        }

        bool ShaderProgram::setTextureArrayElement(const std::string &textureUnitArrayName, unsigned int index, RenderSystem::Texture *texture)
        {
            auto mapIterator = textureUnitArrays.find(textureUnitArrayName);

            if (mapIterator == textureUnitArrays.end()) {
                return false;
            }

            mapIterator->second.textures[index] = texture;

            return true;
        }

        bool ShaderProgram::setTextureArray(const std::string &textureUnitArrayName, std::vector<RenderSystem::Texture *> textures)
        {
            auto mapIterator = textureUnitArrays.find(textureUnitArrayName);

            if (mapIterator == textureUnitArrays.end()) {
                return false;
            }

            auto &textureUnitArray = mapIterator->second;

            unsigned int counter = 0;

            for (auto &storedTexture : textureUnitArray.textures) {
                storedTexture = textures[counter++];
            }

            return true;
        }

        ShaderProgram::TextureUnit::TextureUnit(Sampler &&sampler, RenderSystem::Texture *texture, unsigned int index)
        :
            sampler(std::move(sampler)), texture(texture), index(index)
        {}

        ShaderProgram::TextureUnit::~TextureUnit() {}

        ShaderProgram::TextureUnit::TextureUnit(TextureUnit &&other)
        :
            sampler(std::move(other.sampler)), texture(std::move(other.texture)), index(other.index)
        {
            other.index = 0;
        }

        ShaderProgram::TextureUnit &ShaderProgram::TextureUnit::operator=(TextureUnit &&other)
        {
            sampler = std::move(other.sampler);
            texture = std::move(other.texture);
            index = other.index;

            other.index = 0;

            return *this;
        }

        ShaderProgram::TextureUnitArray::TextureUnitArray(Sampler &&sampler, std::vector<RenderSystem::Texture *> &&textures, unsigned int firstIndexInclusive, unsigned int lastIndexExclusive)
        :
            sampler(std::move(sampler)), textures(std::move(textures)), firstIndexInclusive(firstIndexInclusive), lastIndexExclusive(lastIndexExclusive)
        {}

        ShaderProgram::TextureUnitArray::~TextureUnitArray() {}

        ShaderProgram::TextureUnitArray::TextureUnitArray(TextureUnitArray &&other)
        :
            sampler(std::move(other.sampler)), textures(std::move(other.textures)), firstIndexInclusive(other.firstIndexInclusive), lastIndexExclusive(other.lastIndexExclusive)
        {
            other.firstIndexInclusive = 0;
            other.lastIndexExclusive = 0;
        }

        ShaderProgram::TextureUnitArray &ShaderProgram::TextureUnitArray::operator=(TextureUnitArray &&other)
        {
            sampler = std::move(other.sampler);
            textures = std::move(other.textures);
            firstIndexInclusive = other.firstIndexInclusive;
            lastIndexExclusive = other.lastIndexExclusive;

            other.firstIndexInclusive = 0;
            other.lastIndexExclusive = 0;

            return *this;
        }
    }
}
