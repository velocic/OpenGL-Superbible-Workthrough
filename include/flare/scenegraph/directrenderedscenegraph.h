#ifndef FLARE_SCENEGRAPH_DIRECTRENDEREDSCENEGRAPH_H
#define FLARE_SCENEGRAPH_DIRECTRENDEREDSCENEGRAPH_H

#include <memory>

#include <flare/scenegraph/scenegraph.h>

namespace Flare
{
    namespace SceneGraph
    {
        class DirectRenderedSceneGraph : public SceneGraph
        {
            private:
                std::unique_ptr<Node> rootNode;
                size_t nextNameToAssign = 0;

            public:
                DirectRenderedSceneGraph();
                virtual ~DirectRenderedSceneGraph();
                DirectRenderedSceneGraph(DirectRenderedSceneGraph &&other);
                DirectRenderedSceneGraph &operator=(DirectRenderedSceneGraph &&other);
                DirectRenderedSceneGraph(const DirectRenderedSceneGraph &other) = delete;
                DirectRenderedSceneGraph &operator=(const DirectRenderedSceneGraph &other) = delete;

                virtual void destroy() override;
                virtual Node *getRootNode() const override;
                virtual void render() override;
                virtual size_t requestName() override;
        };
    }
}

#endif
