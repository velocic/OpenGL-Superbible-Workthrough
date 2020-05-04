#ifndef FLARE_GL_TEXTUREBUFFER_H
#define FLARE_GL_TEXTUREBUFFER_H

#include <GL/gl3w.h>

#include <flare/gl/texture.h>
#include <flare/rendersystem/texturebuffer.h>

namespace Flare
{
    namespace GL
    {
        class TextureBuffer : public Texture, public RenderSystem::TextureBuffer
        {
            public:
                TextureBuffer(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth);
                virtual ~TextureBuffer() override;
                TextureBuffer(TextureBuffer &&other);
                TextureBuffer &operator=(TextureBuffer &&other);
                TextureBuffer(const TextureBuffer &other) = delete;
                TextureBuffer &operator=(const TextureBuffer &other) = delete;

                virtual void attachBufferStorage(const RenderSystem::Buffer &buffer) override;

                virtual void destroy() override;
                virtual void initialize() override;
                virtual void bind(GLuint textureUnitIndex) override;
                virtual void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
                virtual void bindAsWritableFromShader(RenderSystem::RSuint unit, RenderSystem::RSint level, RenderSystem::RSboolean layered, RenderSystem::RSint layer, RenderSystem::RSenum access, RenderSystem::RSenum format) override;
        };
    }
}

#endif
