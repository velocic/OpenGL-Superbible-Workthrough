#include <flare/rendersystem/wrappers.h>

#ifdef RENDERSYSTEM_GL4
#include <GL/gl3w.h>
#endif

namespace Flare
{
    namespace RenderSystem
    {
#ifdef RENDERSYSTEM_GL4
        void setUniform1f(RSint location, RSfloat v0)
        {
            glUniform1f(location, v0);
        }

        void setUniform2f(RSint location, RSfloat v0, RSfloat v1)
        {
            glUniform2f(location, v0, v1);
        }

        void setUniform3f(RSint location, RSfloat v0, RSfloat v1, RSfloat v2)
        {
            glUniform3f(location, v0, v1, v2);
        }

        void setUniform4f(RSint location, RSfloat v0, RSfloat v1, RSfloat v2, RSfloat v3)
        {
            glUniform4f(location, v0, v1, v2, v3);
        }

        void setUniform1i(RSint location, RSint v0)
        {
            glUniform1i(location, v0);
        }

        void setUniform2i(RSint location, RSint v0, RSint v1)
        {
            glUniform2i(location, v0, v1);
        }

        void setUniform3i(RSint location, RSint v0, RSint v1, RSint v2)
        {
            glUniform3i(location, v0, v1, v2);
        }

        void setUniform4i(RSint location, RSint v0, RSint v1, RSint v2, RSint v3)
        {
            glUniform4i(location, v0, v1, v2, v3);
        }

        void setUniform1ui(RSint location, RSuint v0)
        {
            glUniform1ui(location, v0);
        }

        void setUniform2ui(RSint location, RSuint v0, RSuint v1)
        {
            glUniform2ui(location, v0, v1);
        }

        void setUniform3ui(RSint location, RSuint v0, RSuint v1, RSuint v2)
        {
            glUniform3ui(location, v0, v1, v2);
        }

        void setUniform4ui(RSint location, RSuint v0, RSuint v1, RSuint v2, RSuint v3)
        {
            glUniform4ui(location, v0, v1, v2, v3);
        }

        void setUniform1fv(RSint location, RSsizei count, const RSfloat *value)
        {
            glUniform1fv(location, count, value);
        }

        void setUniform2fv(RSint location, RSsizei count, const RSfloat *value)
        {
            glUniform2fv(location, count, value);
        }

        void setUniform3fv(RSint location, RSsizei count, const RSfloat *value)
        {
            glUniform3fv(location, count, value);
        }

        void setUniform4fv(RSint location, RSsizei count, const RSfloat *value)
        {
            glUniform4fv(location, count, value);
        }

        void setUniform1iv(RSint location, RSsizei count, const RSint *value)
        {
            glUniform1iv(location, count, value);
        }

        void setUniform2iv(RSint location, RSsizei count, const RSint *value)
        {
            glUniform2iv(location, count, value);
        }

        void setUniform3iv(RSint location, RSsizei count, const RSint *value)
        {
            glUniform3iv(location, count, value);
        }

        void setUniform4iv(RSint location, RSsizei count, const RSint *value)
        {
            glUniform4iv(location, count, value);
        }

        void setUniform1uiv(RSint location, RSsizei count, const RSuint *value)
        {
            glUniform1uiv(location, count, value);
        }

        void setUniform2uiv(RSint location, RSsizei count, const RSuint *value)
        {
            glUniform2uiv(location, count, value);
        }

        void setUniform3uiv(RSint location, RSsizei count, const RSuint *value)
        {
            glUniform3uiv(location, count, value);
        }

        void setUniform4uiv(RSint location, RSsizei count, const RSuint *value)
        {
            glUniform4uiv(location, count, value);
        }

        void setUniformMatrix2fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix2fv(location, count, transpose, value);
        }

        void setUniformMatrix3fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix3fv(location, count, transpose, value);
        }

        void setUniformMatrix4fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix4fv(location, count, transpose, value);
        }

        void setUniformMatrix2x3fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix2x3fv(location, count, transpose, value);
        }

        void setUniformMatrix3x2fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix3x2fv(location, count, transpose, value);
        }

        void setUniformMatrix2x4fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix2x4fv(location, count, transpose, value);
        }

        void setUniformMatrix4x2fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix4x2fv(location, count, transpose, value);
        }

        void setUniformMatrix3x4fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix3x4fv(location, count, transpose, value);
        }

        void setUniformMatrix4x3fv(RSint location, RSsizei count, RSboolean transpose, const RSfloat *value)
        {
            glUniformMatrix4x3fv(location, count, transpose, value);
        }
#endif
    }
}
