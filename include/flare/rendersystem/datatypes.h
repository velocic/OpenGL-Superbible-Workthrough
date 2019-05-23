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
        using RSint = GLint;
        using RSintptr = GLintptr;
        using RSsizei =  GLsizei;
        using RSsizeiptr = GLsizeiptr;
        using RSuint = GLuint;
#endif
    }
}

#endif
