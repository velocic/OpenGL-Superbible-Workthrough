#ifndef FLARE_SCENEGRAPH_INDIRECTRENDEREDSCENEGRAPH_H
#define FLARE_SCENEGRAPH_INDIRECTRENDEREDSCENEGRAPH_H

#include <memory>

#include <flare/rendersystem/resizablebuffer.h>
#include <flare/scenegraph/scenegraph.h>

namespace Flare
{
    namespace SceneGraph
    {
        class IndirectRenderedSceneGraph : public SceneGraph
        {
            private:
                RenderSystem::ResizableBuffer indirectRenderCommandsBuffer;
                std::unique_ptr<Node> rootNode;
                size_t nextNameToAssign = 0;
            public:
                IndirectRenderedSceneGraph();
                virtual ~IndirectRenderedSceneGraph();
                IndirectRenderedSceneGraph(IndirectRenderedSceneGraph &&other);
                IndirectRenderedSceneGraph &operator=(IndirectRenderedSceneGraph &&other);
                IndirectRenderedSceneGraph(const IndirectRenderedSceneGraph &other) = delete;
                IndirectRenderedSceneGraph &operator=(const IndirectRenderedSceneGraph &other) = delete;

                virtual void destroy() override;
                virtual Node *getRootNode() const override;
                virtual void render() override;
                virtual size_t requestName() override;
        };
    }
}

#endif
