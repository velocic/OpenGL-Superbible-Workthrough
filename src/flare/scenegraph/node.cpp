#include <flare/scenegraph/node.h>

namespace Flare
{
    namespace SceneGraph
    {
        Node::Node(RenderSystem::BufferManager &bufferManager, Node *parent)
        {
        }

        Node::Node(RenderSystem::BufferManager &bufferManager, Node *parent, Model *model)
        {
        }

        Node::Node(RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize)
        {
        }

        Node::Node(RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize, Model *model)
        {
        }

        Node::~Node()
        {
        }

        Node::Node(const Node &other)
        {
        }

        Node::Node(Node &&other)
        {
        }

        Node &Node::operator=(const Node &other)
        {
        }

        Node &Node::operator=(Node &&other)
        {
        }

        void Node::destroy()
        {
        }

        glm::mat4 Node::getLocalTransform(size_t instanceId) const
        {
        }

        void Node::setParent(Node *newParent)
        {
        }

        void Node::setModel(Model *newModel)
        {
        }

        void Node::translateNode(const glm::vec3 &translation)
        {
        }

        void Node::rotateNode(const glm::vec3 &rotation)
        {
        }

        void Node::scaleNode(const glm::vec3 &scale)
        {
        }

        void Node::translateInstance(size_t index, const glm::vec3 &translation)
        {
        }

        void Node::rotateInstance(size_t index, const glm::vec3 &rotation)
        {
        }

        void Node::scaleInstance(size_t index, const glm::vec3 &scale)
        {
        }

        size_t Node::addInstance()
        {
        }

        void Node::addChildNode(Node *child)
        {
        }

        void Node::removeInstance(size_t index)
        {
        }

        void Node::removeAllInstances()
        {
        }

        void Node::removeChildNode(Node *child)
        {
        }

        void Node::removeAllChildren()
        {
        }
    }
}
