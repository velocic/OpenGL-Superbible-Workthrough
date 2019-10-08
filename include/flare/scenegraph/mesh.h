#ifndef FLARE_SCENEGRAPH_MESH_H
#define FLARE_SCENEGRAPH_MESH_H

#include <flare/rendersystem/shadermanager.h>
#include <flare/rendersystem/buffer.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Mesh
        {
            public:
                virtual ~Mesh() {}
                virtual void destroy() = 0;
                virtual void bind(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer) = 0;
                virtual void render(size_t instanceCount) = 0;
        };
    }
}

#endif
