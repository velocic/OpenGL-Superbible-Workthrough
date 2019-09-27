#ifndef FLARE_RENDERSYSTEM_WRAPPERS_H
#define FLARE_RENDERSYSTEM_WRAPPERS_H

#include <flare/rendersystem/datatypes.h>

namespace Flare
{
    namespace RenderSystem
    {
        void setUniform1f(RSint location, RSfloat v0);
        void setUniform2f(RSint location, RSfloat v0, RSfloat v1);
        void setUniform3f(RSint location, RSfloat v0, RSfloat v1, RSfloat v2);
        void setUniform4f(RSint location, RSfloat v0, RSfloat v1, RSfloat v2, RSfloat v3);

        void setUniform1i(RSint location, RSint v0);
        void setUniform2i(RSint location, RSint v0, RSint v1);
        void setUniform3i(RSint location, RSint v0, RSint v1, RSint v2);
        void setUniform4i(RSint location, RSint v0, RSint v1, RSint v2, RSint v3);

        void setUniform1ui(RSint location, RSuint v0);
        void setUniform2ui(RSint location, RSuint v0, RSuint v1);
        void setUniform3ui(RSint location, RSuint v0, RSuint v1, RSuint v2);
        void setUniform4ui(RSint location, RSuint v0, RSuint v1, RSuint v2, RSuint v3);

        void setUniform1fv(RSint location, RSsizei count, const RSfloat *value);
        void setUniform2fv(RSint location, RSsizei count, const RSfloat *value);
        void setUniform3fv(RSint location, RSsizei count, const RSfloat *value);
        void setUniform4fv(RSint location, RSsizei count, const RSfloat *value);

        void setUniform1iv(RSint location, RSsizei count, const RSint *value);
        void setUniform2iv(RSint location, RSsizei count, const RSint *value);
        void setUniform3iv(RSint location, RSsizei count, const RSint *value);
        void setUniform4iv(RSint location, RSsizei count, const RSint *value);

        void setUniform1uiv(RSint location, RSsizei count, const RSuint *value);
        void setUniform2uiv(RSint location, RSsizei count, const RSuint *value);
        void setUniform3uiv(RSint location, RSsizei count, const RSuint *value);
        void setUniform4uiv(RSint location, RSsizei count, const RSuint *value);

        void setUniformMatrix2fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix3fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix4fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix2x3fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix3x2fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix2x4fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix4x2fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix3x4fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
        void setUniformMatrix4x3fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value);
    }
}

#endif
