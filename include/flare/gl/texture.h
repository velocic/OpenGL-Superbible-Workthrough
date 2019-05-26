#ifndef FLARE_GL_TEXTURE_H
#define FLARE_GL_TEXTURE_H

#include <GL/gl3w.h>

#include <flare/rendersystem/texture.h>

namespace Flare
{
    namespace GL
    {
        class Texture : public RenderSystem::Texture
        {
            protected:
                GLuint glTexture = 0;
                GLsizei numMipmapLevels = 1;
                GLenum internalFormat = GL_RGBA;
                GLsizei textureWidth = 0;
            public:
                Texture(GLsizei numMipmapLevels, GLenum internalFormat, GLsizei textureWidth);
                virtual ~Texture();
                Texture(Texture&& other);
                Texture& operator=(Texture&& other);
                Texture(const Texture& other) = delete;
                Texture& operator=(const Texture& other) = delete;

                virtual void textureParameterf(GLenum pname, GLfloat param);
                virtual void textureParameteri(GLenum pname, GLint param);

                virtual void setParameter(RenderSystem::RSenum name, RenderSystem::RSfloat value) override;
                virtual void setParameter(RenderSystem::RSenum name, RenderSystem::RSint value) override;

                virtual void destroy() override = 0;
                virtual void initialize() override = 0;
                virtual void bind(GLuint textureUnitIndex) override = 0;
                virtual void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) = 0;
                virtual void bindAsWritableFromShader(RenderSystem::RSuint unit, RenderSystem::RSint level, RenderSystem::RSboolean layered, RenderSystem::RSint layer, RenderSystem::RSenum access, RenderSystem::RSenum format) override = 0;
        };
    }
}

#endif
