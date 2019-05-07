#ifndef FLARE_GL_TEXTURE2D_H
#define FLARE_GL_TEXTURE2D_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/gl/texture.h>

namespace Flare
{
    namespace GL
    {
        class Texture2D : public Texture
        {
            private:
                GLuint textureBuffer = 0;
                GLsizei numMipmapLevels = 0;
                GLenum internalFormat = GL_RGBA;
                GLsizei textureWidth = 0;
                GLsizei textureHeight = 0;
            public:
                Texture(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight);
                void initialize() override;
                void destroy() override;
                void bind(GLenum target) override;
        };
    }
}

#endif
