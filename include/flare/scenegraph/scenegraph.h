#ifndef FLARE_SCENEGRAPH_SCENEGRAPH_H
#define FLARE_SCENEGRAPH_SCENEGRAPH_H

#include <memory>
#include <unordered_map>

#include <flare/rendersystem/resizablebuffer.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Node;
        class SceneGraph
        {
            private:
                // RenderSystem::ResizableBuffer indirectRenderCommandsBuffer;
                // std::unique_ptr<Node> rootNode;
                // size_t nextNameToAssign = 0;

            public:
                // SceneGraph();
                // ~SceneGraph();
                // SceneGraph(SceneGraph &&other);
                // SceneGraph &operator=(SceneGraph &&other);
                // SceneGraph(const SceneGraph &other) = delete;
                // SceneGraph &operator=(const SceneGraph &other) = delete;
                //
                virtual void destroy() = 0;
                virtual Node *getRootNode() const = 0;
                virtual void render() = 0;
                virtual void renderIndirect() = 0;

                //Used by Nodes for fetching a unique numeric id for nodes on creation.
                //Public so that nodes can request this in their copy constructor
                //Avoid using for other scenarios
                virtual size_t requestName() = 0;
        };
    }
}

#endif
