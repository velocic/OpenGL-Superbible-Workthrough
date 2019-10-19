#ifndef FLARE_RENDERSYSTEM_DATATYPES_H
#define FLARE_RENDERSYSTEM_DATATYPES_H

#ifdef RENDERSYSTEM_GL4
#include <GL/gl3w.h>
#endif

namespace Flare
{
    namespace RenderSystem
    {
#ifdef RENDERSYSTEM_GL4
        using RSbitfield = GLbitfield;
        using RSboolean = GLboolean;
        using RSbyte = GLbyte;
        using RSdouble = GLdouble;
        using RSenum = GLenum;
        using RSfloat = GLfloat;
        using RShalf = GLhalf;
        using RSint = GLint;
        using RSintptr = GLintptr;
        using RSshort = GLshort;
        using RSsizei =  GLsizei;
        using RSsizeiptr = GLsizeiptr;
        using RSubyte = GLubyte;
        using RSuint = GLuint;
        using RSushort = GLushort;

        //GLenum types
        constexpr auto RS_BYTE = GL_BYTE;
        constexpr auto RS_DOUBLE = GL_DOUBLE;
        constexpr auto RS_FALSE = GL_FALSE;
        constexpr auto RS_FLOAT = GL_FLOAT;
        constexpr auto RS_HALF_FLOAT = GL_HALF_FLOAT;
        constexpr auto RS_INT = GL_INT;
        constexpr auto RS_INT_2_10_10_10_REV = GL_INT_2_10_10_10_REV;
        constexpr auto RS_SHORT = GL_SHORT;
        constexpr auto RS_TRUE = GL_TRUE;
        constexpr auto RS_UNSIGNED_BYTE = GL_UNSIGNED_BYTE;
        constexpr auto RS_UNSIGNED_INT = GL_UNSIGNED_INT;
        constexpr auto RS_UNSIGNED_INT_10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV;
        constexpr auto RS_UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV;
        constexpr auto RS_UNSIGNED_SHORT = GL_UNSIGNED_SHORT;

        //Buffer usage flags
        constexpr auto RS_DYNAMIC_STORAGE_BIT = GL_DYNAMIC_STORAGE_BIT;
        constexpr auto RS_MAP_READ_BIT = GL_MAP_READ_BIT;
        constexpr auto RS_MAP_WRITE_BIT = GL_MAP_WRITE_BIT;
        constexpr auto RS_MAP_PERSISTENT_BIT = GL_MAP_PERSISTENT_BIT;
        constexpr auto RS_MAP_COHERENT_BIT = GL_MAP_COHERENT_BIT;
        constexpr auto RS_CLIENT_STORAGE_BIT = GL_CLIENT_STORAGE_BIT;

        //Texture data format enums
        constexpr auto RS_RGBA = GL_RGBA;
        constexpr auto RS_RGBA8 = GL_RGBA8;

        struct DrawElementsIndirectCommand {
            GLuint count;
            GLuint instanceCount;
            GLuint firstIndex;
            GLuint baseVertex;
            GLuint baseInstance;
        };
#endif
    }
}

#endif
