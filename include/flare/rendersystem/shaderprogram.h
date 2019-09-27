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
                virtual ~ShaderProgram() {}

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

                template<typename... Ts>
                void setUniform(RSint location, Ts... values) {
                    constexpr auto numArgs = sizeof...(Ts);

                    static_assert(numArgs >= 1 && numArgs <= 4, "setUniform called with incorrect number of values");
                    static_assert((std::is_arithmetic<Ts>::value && ...),"setUniform called with invalid type");

                    if constexpr ((std::is_same<Ts, RSfloat>::value && ...)) {
                        if constexpr (numArgs == 1) { glUniform1f(location, values...); }
                        if constexpr (numArgs == 2) { glUniform2f(location, values...); }
                        if constexpr (numArgs == 3) { glUniform3f(location, values...); }
                        if constexpr (numArgs == 4) { glUniform4f(location, values...); }
                    } else if constexpr ((std::is_same<Ts, RSint>::value && ...)) {
                        if constexpr (numArgs == 1) { glUniform1i(location, values...); }
                        if constexpr (numArgs == 2) { glUniform2i(location, values...); }
                        if constexpr (numArgs == 3) { glUniform3i(location, values...); }
                        if constexpr (numArgs == 4) { glUniform4i(location, values...); }
                    } else if constexpr ((std::is_same<Ts, RSuint>::value && ...)) {
                        if constexpr (numArgs == 1) { glUniform1ui(location, values...); }
                        if constexpr (numArgs == 2) { glUniform1ui(location, values...); }
                        if constexpr (numArgs == 3) { glUniform1ui(location, values...); }
                        if constexpr (numArgs == 4) { glUniform1ui(location, values...); }
                    }
                }

                template<RSsizei Count, typename T>
                void setUniformv(RSint location, const T *value) {
                    static_assert(Count >= 1 && Count <= 4, "setUniformv called with unsupported vector size");
                    static_assert(std::is_arithmetic<T>::value, "setUniformv called with invalid type");

                    if constexpr (std::is_same<T, RSfloat>::value) {
                        if constexpr (Count == 1) { glUniform1fv(location, Count, value); }
                        if constexpr (Count == 2) { glUniform2fv(location, Count, value); }
                        if constexpr (Count == 3) { glUniform3fv(location, Count, value); }
                        if constexpr (Count == 4) { glUniform4fv(location, Count, value); }
                    } else if constexpr (std::is_same<T, RSint>::value) {
                        if constexpr (Count == 1) { glUniform1iv(location, Count, value); }
                        if constexpr (Count == 2) { glUniform2iv(location, Count, value); }
                        if constexpr (Count == 3) { glUniform3iv(location, Count, value); }
                        if constexpr (Count == 4) { glUniform4iv(location, Count, value); }
                    } else if constexpr (std::is_same<T, RSuint>::value) {
                        if constexpr (Count == 1) { glUniform1uiv(location, Count, value); }
                        if constexpr (Count == 2) { glUniform2uiv(location, Count, value); }
                        if constexpr (Count == 3) { glUniform3uiv(location, Count, value); }
                        if constexpr (Count == 4) { glUniform4uiv(location, Count, value); }
                    }
                }

                template<size_t Rows, size_t Columns, RSsizei Count>
                void setUniformMatrix(RSint location, RSboolean transpose, const RSfloat *value) {
                    static_assert(
                        (Rows == 2 && Columns == 2) ||
                        (Rows == 2 && Columns == 3) ||
                        (Rows == 2 && Columns == 4) ||
                        (Rows == 3 && Columns == 2) ||
                        (Rows == 3 && Columns == 3) ||
                        (Rows == 3 && Columns == 4) ||
                        (Rows == 4 && Columns == 2) ||
                        (Rows == 4 && Columns == 3) ||
                        (Rows == 4 && Columns == 4),
                        "setUniformMatrix called with unsupported matrix layout"
                    );

                    if constexpr (Rows == 2) {
                        if constexpr (Columns == 2) {
                            glUniformMatrix2fv(location, Count, transpose, value);
                        } else if constexpr (Columns == 3) {
                            glUniformMatrix2x3fv(location, Count, transpose, value);
                        } else if constexpr (Columns == 4) {
                            glUniformMatrix2x4fv(location, Count, transpose, value);
                        }
                    } else if constexpr (Rows == 3) {
                        if constexpr (Columns == 2) {
                            glUniformMatrix3x2fv(location, Count, transpose, value);
                        } else if constexpr (Columns == 3) {
                            glUniformMatrix3fv(location, Count, transpose, value);
                        } else if constexpr (Columns == 4) {
                            glUniformMatrix3x4fv(location, Count, transpose, value);
                        }
                    } else if constexpr (Rows == 4) {
                        if constexpr (Columns == 2) {
                            glUniformMatrix4x2fv(location, Count, transpose, value);
                        } else if constexpr (Columns == 3) {
                            glUniformMatrix4x3fv(location, Count, transpose, value);
                        } else if constexpr (Columns == 4) {
                            glUniformMatrix4fv(location, Count, transpose, value);
                        }
                    }
                }
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
