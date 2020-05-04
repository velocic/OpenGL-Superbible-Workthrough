#include <flare/gl/texturebuffer.h>

namespace Flare
{
    namespace GL
    {
        TextureBuffer::TextureBuffer(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth)
        :
            Texture(numMipmapLevels, internalFormat, textureWidth)
        {
            initialize();
        }

        TextureBuffer::~TextureBuffer()
        {
            destroy();
        }

        TextureBuffer::TextureBuffer(TextureBuffer &&other)
        :
            Texture(std::move(other))
        {
        }

        TextureBuffer &TextureBuffer::operator=(TextureBuffer &&other)
        {
            Texture::operator=(std::move(other));

            return *this;
        }

        void TextureBuffer::attachBufferStorage(const RenderSystem::Buffer &buffer)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to attach storage to an OpenGL texture that is uninitialized.");
            }

            glTextureBuffer(glTexture, internalFormat, buffer.getId());
        }

        void TextureBuffer::destroy()
        {
            glDeleteTextures(1, &glTexture);
        }

        void TextureBuffer::initialize()
        {
            if (glTexture != 0) {
                throw std::runtime_error("Attempting to initialize an OpenGL texture that's already initialized.");
            }

            glCreateTextures(GL_TEXTURE_BUFFER, 1, &glTexture);
        }

        void TextureBuffer::bind(GLuint textureUnitIndex)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to bind an OpenGL texture that's uninitialized.");
            }

            glBindTextureUnit(textureUnitIndex, glTexture);
        }

        void TextureBuffer::bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
        {
            if (glTexture == 0) {
                throw std::runtime_error("Attempting to bind an OpenGL image texture that's uninitialized.");
            }

            glBindImageTexture(
                unit,
                glTexture,
                level,
                layered,
                layer,
                access,
                format
            );
        }

        void TextureBuffer::bindAsWritableFromShader(RenderSystem::RSuint unit, RenderSystem::RSint level, RenderSystem::RSboolean layered, RenderSystem::RSint layer, RenderSystem::RSenum access, RenderSystem::RSenum format)
        {
            bindImageTexture(unit, level, layered, layer, access, format);
        }
    }
}
