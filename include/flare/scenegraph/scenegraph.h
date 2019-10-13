#ifndef FLARE_SCENEGRAPH_SCENEGRAPH_H
#define FLARE_SCENEGRAPH_SCENEGRAPH_H

#include <flare/scenegraph/node.h>

namespace Flare
{
    namespace SceneGraph
    {
        class SceneGraph
        {
            private:
                std::vector<Node> nodes;
                size_t nextNameToAssign = 0;

                size_t requestName();
            public:
                Node *createNode(Node *parent);
                Node *createNode(Node *parent, Model *model);
                Node *createNode(Node *parent, size_t instanceCountReserveSize);
                Node *createNode(Node *parent, size_t instanceCountReserveSize, Model *model);
        };
    }
}

#endif
