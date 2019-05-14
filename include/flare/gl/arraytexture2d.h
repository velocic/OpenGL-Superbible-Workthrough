#ifndef FLARE_GL_ARRAYTEXTURE2D_H
#define FLARE_GL_ARRAYTEXTURE2D_H

#include <GL/gl3w.h>
#include <flare/gl/texture.h>

namespace Flare
{
    namespace GL
    {
        class ArrayTexture2D : public Texture
        {
            protected:
                GLsizei textureHeight = 0;
                GLsizei textureDepth = 0;
            public:
                ArrayTexture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight, GLsizei textureDepth);
                virtual ~ArrayTexture2D() override;
                ArrayTexture2D(ArrayTexture2D&& other);
                ArrayTexture2D& operator=(ArrayTexture2D&& other);
                ArrayTexture2D(const ArrayTexture2D& other) = delete;
                ArrayTexture2D& operator=(const ArrayTexture2D& other) = delete;

                void textureSubImage3D(GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, bool generateMipmaps);
                void clearTexSubImage(GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);

                void initialize() override;
                void destroy() override;
                void bind(GLuint textureUnitIndex) override;
        };
    }
}

#endif
