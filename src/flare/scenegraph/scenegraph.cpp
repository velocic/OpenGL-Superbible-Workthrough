#include <flare/scenegraph/scenegraph.h>

#include <algorithm>

namespace Flare
{
    namespace SceneGraph
    {
        size_t SceneGraph::requestName()
        {
            return nextNameToAssign++;
        }

        SceneGraph::SceneGraph(SceneGraph &&other)
        :
            nodes(std::move(other.nodes)),
            bufferManager(std::move(other.bufferManager)),
            nextNameToAssign(std::exchange(other.nextNameToAssign, 0))
        {
        }

        SceneGraph &SceneGraph::operator=(SceneGraph &&other)
        {
            nodes = std::move(other.nodes);
            bufferManager = std::move(other.bufferManager);
            nextNameToAssign = std::exchange(other.nextNameToAssign, 0);
        }

        Node *SceneGraph::createNode(Node *parent)
        {
            auto newNodeName = requestName();
            auto newNode = std::unique_ptr<Node>(new Node(*this, bufferManager, newNodeName, parent));
            auto result = newNode.get();
            nodes.insert_or_assign(newNodeName, std::move(newNode));
            return result;
        }

        Node *SceneGraph::createNode(Node *parent, Model *model)
        {
            auto newNodeName = requestName();
            auto newNode = std::unique_ptr<Node>(new Node(*this, bufferManager, newNodeName, parent, model));
            auto result = newNode.get();
            nodes.insert_or_assign(newNodeName, std::move(newNode));
            return result;
        }

        Node *SceneGraph::createNode(Node *parent, size_t instanceCountReserveSize)
        {
            auto newNodeName = requestName();
            auto newNode = std::unique_ptr<Node>(new Node(*this, bufferManager, newNodeName, parent, instanceCountReserveSize));
            auto result = newNode.get();
            nodes.insert_or_assign(newNodeName, std::move(newNode));
            return result;
        }

        Node *SceneGraph::createNode(Node *parent, size_t instanceCountReserveSize, Model *model)
        {
            auto newNodeName = requestName();
            auto newNode = std::unique_ptr<Node>(new Node(*this, bufferManager, newNodeName, parent, instanceCountReserveSize, model));
            auto result = newNode.get();
            nodes.insert_or_assign(newNodeName, std::move(newNode));
            return result;
        }

        void SceneGraph::destroyNode(Node *target)
        {
            nodes.erase(target->getName());
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent)
        :
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent)
        {
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, Model *model)
        :
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent),
            model(model)
        {
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, size_t instanceCountReserveSize)
        :
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent)
        {
            setParallelBufferSizes(instanceCountReserveSize);
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, size_t instanceCountReserveSize, Model *model)
        :
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent),
            model(model)
        {
            setParallelBufferSizes(instanceCountReserveSize);
        }

        Node::~Node()
        {
            destroy();
        }

        Node::Node(const Node &other)
        :
            TRSData(other.TRSData),
            instanceData(other.instanceData),
            sceneGraph(other.sceneGraph),
            bufferManager(other.bufferManager),
            parent(other.parent),
            model(other.model)
        {
            name = sceneGraph->requestName();
            deepCopyChildrenOfOtherNode(children, other);
            copyModelMatrixBufferOfOtherNode(other);
        }

        Node::Node(Node &&other)
        :
            TRSData(std::move(other.TRSData)),
            instanceData(std::move(other.instanceData)),
            children(std::move(other.children)),
            name(std::exchange(other.name, 0)),
            sceneGraph(std::exchange(other.sceneGraph, nullptr)),
            bufferManager(std::exchange(other.bufferManager, nullptr)),
            modelMatrixBuffer(std::exchange(other.modelMatrixBuffer, nullptr)),
            parent(std::exchange(other.parent, nullptr)),
            model(std::exchange(other.model, nullptr))
        {
        }

        Node &Node::operator=(const Node &other)
        {
            TRSData = other.TRSData;
            instanceData = other.instanceData;
            sceneGraph = other.sceneGraph;
            bufferManager = other.bufferManager;
            parent = other.parent;
            model = other.model;

            name = sceneGraph->requestName();
            deepCopyChildrenOfOtherNode(children, other);
            copyModelMatrixBufferOfOtherNode(other);

            return *this;
        }

        Node &Node::operator=(Node &&other)
        {
            TRSData = std::move(other.TRSData);
            instanceData = std::move(other.instanceData);
            children = std::move(other.children);
            name = std::exchange(other.name, 0);
            sceneGraph = std::exchange(other.sceneGraph, nullptr);
            bufferManager = std::exchange(other.bufferManager, nullptr);
            modelMatrixBuffer = std::exchange(other.modelMatrixBuffer, nullptr);
            parent = std::exchange(other.parent, nullptr);
            model = std::exchange(other.model, nullptr);

            return *this;
        }

        void Node::destroy()
        {
            for (auto child : children) {
                child->destroy();
            }
            bufferManager->destroy(nodeBaseName + std::to_string(name));
        }

        size_t Node::getName() const
        {
            return name;
        }

        glm::mat4 Node::getNodeLocalTransform() const
        {
            return TRSData.translation * TRSData.rotation * TRSData.scale;
        }

        glm::mat4 Node::getInstanceLocalTransform(size_t instanceId) const
        {
            const auto &instance = instanceData.TRSData[instanceId];
            return (TRSData.translation * TRSData.rotation * TRSData.scale)
                * (instance.translation * instance.rotation * instance.scale);
        }

        void Node::setParent(Node *newParent)
        {
            parent->notifyChildRemoved(this);
            parent = newParent;
        }

        void Node::setModel(Model *newModel)
        {
            model = newModel;
        }

        void Node::translateNode(const glm::vec3 &translation)
        {
            TRSData.translation = glm::translate(
                TRSData.translation,
                translation
            );
        }

        void Node::rotateNode(float angleRadians, const glm::vec3 &axis)
        {
            TRSData.rotation = glm::rotate(
                TRSData.rotation,
                angleRadians,
                axis
            );
        }

        void Node::scaleNode(const glm::vec3 &scale)
        {
            TRSData.scale = glm::scale(
                TRSData.scale,
                scale
            );
        }

        void Node::translateInstance(size_t instanceId, const glm::vec3 &translation)
        {
            auto &instance = instanceData.TRSData[instanceId];
            instance.translation = glm::translate(
                instance.translation,
                translation
            );
        }

        void Node::rotateInstance(size_t instanceId, float angleRadians, const glm::vec3 &axis)
        {
            auto &instance = instanceData.TRSData[instanceId];
            instance.rotation = glm::rotate(
                instance.rotation,
                angleRadians,
                axis
            );
        }

        void Node::scaleInstance(size_t instanceId, const glm::vec3 &scale)
        {
            auto &instance = instanceData.TRSData[instanceId];
            instance.scale = glm::scale(
                instance.scale,
                scale
            );
        }

        size_t Node::addInstance()
        {
            auto newInstanceId = instanceData.numActive;

            if (modelMatrixBuffer == nullptr) {
                setParallelBufferSizes(1);
                instanceData.numActive = 1;
                return newInstanceId;
            }

            const auto modelMatrixBufferSize = modelMatrixBuffer->getSizeInElements();

            if (instanceData.numActive < modelMatrixBufferSize) {
                ++instanceData.numActive;
                return newInstanceId;
            }

            const auto newModelMatrixBufferSize = modelMatrixBufferSize * 2;
            setParallelBufferSizes(newModelMatrixBufferSize);
            ++instanceData.numActive;

            return newInstanceId;
        }

        void Node::addChildNode(Node *child)
        {
            children.push_back(child);
        }

        void Node::removeInstance(size_t instanceId)
        {
            if (instanceData.numActive <= 0) {
                return;
            }

            std::iter_swap(
                instanceData.modelMatrices.begin() + instanceId,
                instanceData.modelMatrices.begin() + std::min(instanceData.numActive - 1, static_cast<size_t>(0))
            );
            std::iter_swap(
                instanceData.TRSData.begin() + instanceId,
                instanceData.TRSData.begin() + std::min(instanceData.numActive - 1, static_cast<size_t>(0))
            );
            --instanceData.numActive;
        }

        void Node::removeAllInstances()
        {
            instanceData.modelMatrices.clear();
            instanceData.TRSData.clear();
        }

        void Node::removeChildNode(Node *removedChild)
        {
            std::remove_if(
                children.begin(),
                children.end(),
                [=](const auto child){
                    return child->getName() == removedChild->getName();
                }
            );
        }

        void Node::removeAllChildren()
        {
            children.clear();
        }

        void Node::copyModelMatrixBufferOfOtherNode(const Node &other)
        {
            if (modelMatrixBuffer != nullptr) {
                bufferManager->destroy(nodeBaseName + std::to_string(name));
            }

            const auto &otherModelMatrixBuffer = *other.modelMatrixBuffer;
            modelMatrixBuffer = bufferManager->create(
                nodeBaseName + std::to_string(name),
                otherModelMatrixBuffer.getContentDescription()
            );
            modelMatrixBuffer->allocateBufferStorage(
                otherModelMatrixBuffer.getSizeInBytes(),
                nullptr,
                otherModelMatrixBuffer.getUsageFlags()
            );
            modelMatrixBuffer->copyRange(
                otherModelMatrixBuffer,
                0,
                0,
                otherModelMatrixBuffer.getSizeInBytes()
            );
        }

        void Node::deepCopyChildrenOfOtherNode(std::vector<Node *> &destination, const Node &other)
        {
            if (other.children.empty()) {
                return;
            }

            for (const auto child : other.children) {
                auto newNode = sceneGraph->createNode(nullptr);
                *newNode = *child;
                destination.push_back(newNode);
                deepCopyChildrenOfOtherNode(newNode->children, *child);
            }
        }

        void Node::notifyChildRemoved(Node *removedChild)
        {
            removeChildNode(removedChild);
        }

        RenderSystem::VertexDataLayout Node::getModelMatrixBufferLayout() const
        {
            return Flare::RenderSystem::VertexDataLayoutBuilder()
                .addMatrixAttribute("mvpMatrix", 4, 4, Flare::RenderSystem::RS_FLOAT, Flare::RenderSystem::RS_FALSE, 0)
                .setStride(sizeof(glm::mat4))
                .setDivisor(1)
                .build();
        }

        void Node::setParallelBufferSizes(size_t size)
        {
            instanceData.modelMatrices.resize(size);
            instanceData.TRSData.resize(size);
            if (modelMatrixBuffer != nullptr) {
                modelMatrixBuffer = bufferManager->resizeElements(nodeBaseName + std::to_string(name), size);
                return;
            }

            modelMatrixBuffer = bufferManager->create(nodeBaseName + std::to_string(name), getModelMatrixBufferLayout());
            modelMatrixBuffer->allocateBufferStorage(
                sizeof(glm::mat4) * size,
                nullptr,
                getModelMatrixBufferUsageFlags()
            );
        }
    }
}
