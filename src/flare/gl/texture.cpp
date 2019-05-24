#include <flare/gl/texture.h>

#include <algorithm>

namespace Flare
{
    namespace GL
    {
        Texture::Texture(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth)
        :
            RenderSystem::Texture(numMipmapLevels, internalFormat, textureWidth),
            numMipmapLevels(numMipmapLevels),
            internalFormat(internalFormat),
            textureWidth(textureWidth)
        {}

        Texture::~Texture()
        {}

        Texture::Texture(Texture&& other)
        :
            RenderSystem::Texture(std::move(other)),
            numMipmapLevels(other.numMipmapLevels),
            internalFormat(other.internalFormat),
            textureWidth(other.textureWidth)
        {
            other.glTexture = 0;
            other.numMipmapLevels = 0;
            other.internalFormat = GL_RGBA;
            other.textureWidth = 0;
        }

        Texture& Texture::operator=(Texture&& other)
        {
            glTexture = other.glTexture;
            numMipmapLevels = other.numMipmapLevels;
            textureWidth = other.textureWidth;

            other.glTexture = 0;
            other.numMipmapLevels = 0;
            other.internalFormat = GL_RGBA;
            other.textureWidth = 0;

            return *this;
        }

        void Texture::textureParameterf(GLenum pname, GLfloat param)
        {
            glTextureParameterf(glTexture, pname, param);
        }

        void Texture::textureParameteri(GLenum pname, GLint param)
        {
            glTextureParameteri(glTexture, pname, param);
        }

        void Texture::setParameter(RenderSystem::RSenum name, RenderSystem::RSfloat value)
        {
            textureParameterf(name, value);
        }

        void Texture::setParameter(RenderSystem::RSenum name, RenderSystem::RSint value)
        {
            textureParameteri(name, value);
        }
    }
}
