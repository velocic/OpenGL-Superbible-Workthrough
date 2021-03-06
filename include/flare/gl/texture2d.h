#ifndef FLARE_GL_TEXTURE2D_H
#define FLARE_GL_TEXTURE2D_H

#include <GL/gl3w.h>

#include <flare/gl/texture.h>
#include <flare/rendersystem/texture2d.h>

namespace Flare
{
    namespace GL
    {
        class Texture2D : public Texture, public RenderSystem::Texture2D
        {
            protected:
                GLsizei textureHeight = 0;
            public:
                Texture2D(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth, GLsizei textureHeight);
                virtual ~Texture2D() override;
                Texture2D(Texture2D &&other);
                Texture2D& operator=(Texture2D &&other);
                Texture2D(const Texture2D &other) = delete;
                Texture2D& operator=(const Texture2D &other) = delete;

                void textureSubImage2D(GLenum format, GLenum type, const GLvoid *pixels, bool generateMipmaps);
                void textureSubImage2D(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, bool generateMipmaps);
                void clearTexSubImage(GLenum format, GLenum type, const void *data);
                void clearTexSubImage(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
                virtual void initialize() override;
                virtual void destroy() override;
                virtual void bind(GLuint textureUnitIndex) override;
                virtual void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) override;

                virtual void bindAsWritableFromShader(RenderSystem::RSuint unit, RenderSystem::RSint level, RenderSystem::RSboolean layered, RenderSystem::RSint layer, RenderSystem::RSenum access, RenderSystem::RSenum format) override;

                virtual void bufferPixelData(RenderSystem::RSenum format, RenderSystem::RSenum type, const void *pixels, RenderSystem::RSboolean generateMipmaps) override;
                virtual void bufferPixelData(RenderSystem::RSint level, RenderSystem::RSint xOffset, RenderSystem::RSint yOffset, RenderSystem::RSsizei width, RenderSystem::RSsizei height, RenderSystem::RSenum format, RenderSystem::RSenum type, const void *pixels, RenderSystem::RSboolean generateMipmaps) override;
                virtual void clearPixelData(RenderSystem::RSenum format, RenderSystem::RSenum type, const void *data) override;
                virtual void clearPixelData(RenderSystem::RSint level, RenderSystem::RSint xOffset, RenderSystem::RSint yOffset, RenderSystem::RSsizei width, RenderSystem::RSsizei height, RenderSystem::RSenum format, RenderSystem::RSenum type, const void *data) override;
        };
    }
}

#endif
