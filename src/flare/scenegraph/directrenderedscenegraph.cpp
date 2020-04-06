#include <flare/scenegraph/directrenderedscenegraph.h>

namespace Flare
{
    namespace SceneGraph
    {
        DirectRenderedSceneGraph::DirectRenderedSceneGraph()
        {
            rootNode = std::unique_ptr<Node>(new Node(*this, requestName(), nullptr));
        }

        DirectRenderedSceneGraph::~DirectRenderedSceneGraph()
        {
        }

        DirectRenderedSceneGraph::DirectRenderedSceneGraph(DirectRenderedSceneGraph &&other)
        :
            rootNode(std::move(other.rootNode)),
            nextNameToAssign(std::exchange(other.nextNameToAssign, 0))
        {
        }

        DirectRenderedSceneGraph &DirectRenderedSceneGraph::operator=(DirectRenderedSceneGraph &&other)
        {
            rootNode = std::move(other.rootNode);
            nextNameToAssign = std::exchange(other.nextNameToAssign, 0);

            return *this;
        }

        void DirectRenderedSceneGraph::destroy()
        {
            rootNode = nullptr;
        }

        Node *DirectRenderedSceneGraph::getRootNode() const
        {
            return rootNode.get();
        }

        void DirectRenderedSceneGraph::render()
        {
            rootNode->render(Math::identityMatrix);
        }

        size_t DirectRenderedSceneGraph::requestName()
        {
            return nextNameToAssign++;
        }
    }
}
