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
        using RSenum = GLenum;
        using RSfloat = GLfloat;
        using RSint = GLint;
        using RSintptr = GLintptr;
        using RSsizei =  GLsizei;
        using RSsizeiptr = GLsizeiptr;
        using RSuint = GLuint;

        //Common flag values
        constexpr auto RS_FLOAT = GL_FLOAT;
        constexpr auto RS_UNSIGNED_INT = GL_UNSIGNED_INT;
        constexpr auto RS_FALSE = GL_FALSE;
        constexpr auto RS_TRUE = GL_TRUE;

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
#endif
    }
}

#endif
