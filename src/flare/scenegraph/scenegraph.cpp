#include <flare/scenegraph/scenegraph.h>

namespace Flare
{
    namespace SceneGraph
    {
        size_t SceneGraph::requestName()
        {
            return nextNameToAssign++;
        }
    }
}
