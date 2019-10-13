#ifndef FLARE_SCENEGRAPH_SCENEGRAPH_H
#define FLARE_SCENEGRAPH_SCENEGRAPH_H

#include <flare/rendersystem/buffermanager.h>
#include <flare/scenegraph/model.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>

namespace Flare
{
    namespace SceneGraph
    {
        class Node;
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

        class Node
        {
            friend class SceneGraph;
            private:
                struct InstanceData
                {
                    std::vector<glm::mat4> worldMatrices;
                    size_t numActive = 0;
                };

                InstanceData instanceData;
                std::vector<Node *> children;
                glm::vec3 translation;
                glm::vec3 rotation;
                glm::vec3 scale;
                size_t name = 0;
                SceneGraph &sceneGraph;
                RenderSystem::BufferManager &bufferManager;
                RenderSystem::Buffer *mvpMatrixBuffer = nullptr;
                Node *parent = nullptr;
                Model *model = nullptr;

                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent);
                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent, Model *model);
                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize);
                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize, Model *model);

                void notifyChildReparented(Node *child);
                RenderSystem::VertexDataLayout getMVPMatrixBufferLayout() const;
            public:
                ~Node();
                Node(const Node &other);
                Node(Node &&other);
                Node &operator=(const Node &other);
                Node &operator=(Node &&other);

                void destroy();

                size_t getName() const;
                glm::mat4 getLocalTransform(size_t instanceId) const;

                void setParent(Node *newParent);
                void setModel(Model *newModel);

                void translateNode(const glm::vec3 &translation);
                void rotateNode(const glm::vec3 &rotation);
                void scaleNode(const glm::vec3 &scale);

                void translateInstance(size_t index, const glm::vec3 &translation);
                void rotateInstance(size_t index, const glm::vec3 &rotation);
                void scaleInstance(size_t index, const glm::vec3 &scale);

                size_t addInstance();
                void addChildNode(Node *child);
                void removeInstance(size_t index);
                void removeAllInstances();
                void removeChildNode(Node *child);
                void removeAllChildren();
        };
    }
}

#endif
