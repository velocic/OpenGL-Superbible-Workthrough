#ifndef FLARE_SCENEGRAPH_NODE_H
#define FLARE_SCENEGRAPH_NODE_H

#include <flare/rendersystem/buffermanager.h>
#include <flare/scenegraph/model.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>

namespace Flare
{
    namespace SceneGraph
    {
        class Node
        {
            private:
                struct InstanceData
                {
                    std::vector<glm::mat4> worldMatrices;
                    size_t numActive = 0;
                };

                std::vector<Node *> children;
                glm::vec3 translation;
                glm::vec3 rotation;
                glm::vec3 scale;
                //const RenderSystem::BufferManager &bufferManager;
                RenderSystem::Buffer *mvpMatrixBuffer = nullptr;
                Node *parent = nullptr;
                Model *renderModel = nullptr;

                void notifyChildReparented(Node *child);
            public:
                Node(RenderSystem::BufferManager &bufferManager, Node *parent);
                Node(RenderSystem::BufferManager &bufferManager, Node *parent, Model *model);
                Node(RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize);
                Node(RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize, Model *model);
                ~Node();
                Node(const Node &other);
                Node(Node &&other);
                Node &operator=(const Node &other);
                Node &operator=(Node &&other);

                void destroy();

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
