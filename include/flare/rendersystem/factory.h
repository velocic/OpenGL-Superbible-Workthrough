#ifndef FLARE_RENDERSYSTEM_FACTORY_H
#define FLARE_RENDERSYSTEM_FACTORY_H

#ifdef RENDERSYSTEM_GL4
#include <flare/gl/buffer.h>
#endif

namespace Flare
{
    namespace RenderSystem
    {
        std::unique_ptr<Buffer> createBuffer(const std::string &name, const VertexDataLayout& bufferContentDescription);
        std::unique_ptr<VertexArray> createVertexArray(const RenderSystem::ShaderProgram *shaderProgram, const std::vector<std::reference_wrapper<const RenderSystem::Buffer>> &linkedBuffers);
    }
}

#endif
