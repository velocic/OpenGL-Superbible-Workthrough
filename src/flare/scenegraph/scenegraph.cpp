#include <flare/scenegraph/scenegraph.h>

namespace Flare
{
    namespace SceneGraph
    {
        size_t SceneGraph::requestName()
        {
            return nextNameToAssign++;
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent)
        :
            sceneGraph(sceneGraph),
            bufferManager(bufferManager),
            parent(parent)
        {
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent, Model *model)
        :
            sceneGraph(sceneGraph),
            bufferManager(bufferManager),
            parent(parent),
            model(model)
        {
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize)
        :
            sceneGraph(sceneGraph),
            bufferManager(bufferManager),
            parent(parent)
        {
            instanceData.worldMatrices.resize(instanceCountReserveSize);
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, Node *parent, size_t instanceCountReserveSize, Model *model)
        :
            sceneGraph(sceneGraph),
            bufferManager(bufferManager),
            parent(parent),
            model(model)
        {
            instanceData.worldMatrices.resize(instanceCountReserveSize);
        }

        Node::~Node()
        {
            destroy();
        }

        Node::Node(const Node &other)
        :
            instanceData(other.instanceData),
            translation(translation),
            rotation(rotation),
            scale(scale),
            sceneGraph(other.sceneGraph),
            bufferManager(other.bufferManager),
            parent(other.parent),
            model(other.model)
        {
            //TODO: deep copy children
            //TODO: request unique name from SceneGraph
        }

        Node::Node(Node &&other)
        :
            instanceData(std::move(other.instanceData)),
            children(std::move(other.children)),
            translation(std::move(other.translation)),
            rotation(std::move(other.rotation)),
            scale(std::move(other.scale)),
            name(std::exchange(other.name, 0)),
            sceneGraph(other.sceneGraph),
            bufferManager(other.bufferManager),
            mvpMatrixBuffer(std::exchange(other.mvpMatrixBuffer, nullptr)),
            parent(std::exchange(other.parent, nullptr)),
            model(std::exchange(other.model, nullptr))
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

        size_t Node::getName() const
        {
            return name;
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

        RenderSystem::VertexDataLayout Node::getMVPMatrixBufferLayout() const
        {
            return Flare::RenderSystem::VertexDataLayoutBuilder()
                .addMatrixAttribute("mvpMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
                .setStride(sizeof(glm::mat4))
                .setDivisor(1)
                .build();
        }
    }
}
