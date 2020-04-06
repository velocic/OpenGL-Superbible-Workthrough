#ifndef FLARE_SCENEGRAPH_SCENEGRAPH_H
#define FLARE_SCENEGRAPH_SCENEGRAPH_H

namespace Flare
{
    namespace SceneGraph
    {
        class Node;
        class SceneGraph
        {
            public:
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
