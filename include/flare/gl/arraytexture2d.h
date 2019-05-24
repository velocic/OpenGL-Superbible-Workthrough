#ifndef FLARE_GL_ARRAYTEXTURE2D_H
#define FLARE_GL_ARRAYTEXTURE2D_H

#include <GL/gl3w.h>

#include <flare/gl/texture.h>
#include <flare/rendersystem/arraytexture2d.h>

namespace Flare
{
    namespace GL
    {
        class ArrayTexture2D : public Texture, public RenderSystem::ArrayTexture2D
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

                virtual void initialize() override;
                virtual void destroy() override;
                virtual void bind(GLuint textureUnitIndex) override;
                virtual void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) override;

                virtual void bindAsWritableFromShader(RenderSystem::RSuint unit, RenderSystem::RSint level, RenderSystem::RSboolean layered, RenderSystem::RSint layer, RenderSystem::RSenum access, RenderSystem::RSenum format) override;

                virtual void bufferPixelData(RenderSystem::RSint level, RenderSystem::RSint xOffset, RenderSystem::RSint yOffset, RenderSystem::RSint zOffset, RenderSystem::RSsizei width, RenderSystem::RSsizei height, RenderSystem::RSenum format, RenderSystem::RSenum type, const void *pixels, RenderSystem::RSboolean generateMipmaps) override;
                virtual void clearPixelData(RenderSystem::RSint level, RenderSystem::RSint xOffset, RenderSystem::RSint yOffset, RenderSystem::RSint zOffset, RenderSystem::RSsizei width, RenderSystem::RSsizei height, RenderSystem::RSenum format, RenderSystem::RSenum type, const void *data) override;
        };
    }
}

#endif
