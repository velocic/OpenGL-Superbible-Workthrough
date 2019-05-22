#ifndef FLARE_SCENEGRAPH_NODE_H
#define FLARE_SCENEGRAPH_NODE_H

namespace Flare
{
    namespace SceneGraph
    {
        class Node
        {
            private:
                struct InstanceData
                {
                    size_t numActive = 0;
                    std::vector<glm::mat4> worldMatrices;
                }

                glm::vec3 translation;
                glm::vec3 rotation;
                glm::vec3 scale;
                Node *parent;
                Model *renderModel;

                void notifyParentDestroyed();
            public:
                Node(Node *parent);
                Node(Node *parent, size_t instanceCountReserveSize);
                ~Node();
                Node(const Node &other);
                Node(Node &&other);
                Node &operator=(const Node &other);
                Node &operator=(Node &&other);

                void setParent(Node *newParent);
                void setModel(Model *newModel);

                void translateNode(const glm::vec3 &translation);
                void rotateNode(const glm::vec3 &rotation);
                void scaleNode(const glm::vec3 &scale);

                void translateInstance(size_t index, const glm::vec3 &translation);
                void rotateInstance(size_t index, const glm::vec3 &rotation);
                void scaleInstance(size_t index, const glm::vec3 &scale);

                size_t addInstance();
                void removeInstance(size_t index);

                // void bufferWorldMatrices()
        };
    }
}

#endif
