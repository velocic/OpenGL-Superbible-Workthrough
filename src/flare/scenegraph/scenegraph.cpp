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

        SceneGraph::SceneGraph()
        {
            rootNode = std::unique_ptr<Node>(new Node(*this, bufferManager, requestName(), nullptr));
        }

        SceneGraph::SceneGraph(SceneGraph &&other)
        :
            rootNode(std::move(other.rootNode)),
            bufferManager(std::move(other.bufferManager)),
            nextNameToAssign(std::exchange(other.nextNameToAssign, 0))
        {
        }

        SceneGraph &SceneGraph::operator=(SceneGraph &&other)
        {
            rootNode = std::move(other.rootNode);
            bufferManager = std::move(other.bufferManager);
            nextNameToAssign = std::exchange(other.nextNameToAssign, 0);

            return *this;
        }

        void SceneGraph::destroy()
        {
            rootNode = nullptr;
        }

        Node *SceneGraph::getRootNode() const
        {
            return rootNode.get();
        }

        void SceneGraph::render()
        {
            rootNode->render(Math::identityMatrix);
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent)
        :
            name(name),
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent)
        {
            if (parent != nullptr) {
                parent->addChildNode(this);
            }

            setParallelBufferSizes(1);
            instanceData.instanceIds[0] = getNextInstanceId();
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, Model *model)
        :
            name(name),
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent),
            model(model)
        {
            if (parent != nullptr) {
                parent->addChildNode(this);
            }

            setParallelBufferSizes(1);
            instanceData.instanceIds[0] = getNextInstanceId();
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, size_t instanceCountReserveSize)
        :
            name(name),
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent)
        {
            if (parent != nullptr) {
                parent->addChildNode(this);
            }

            setParallelBufferSizes(instanceCountReserveSize);
            for (size_t i = 0; i < instanceCountReserveSize; ++i) {
                instanceData.instanceIds[i] = getNextInstanceId();
            }
        }

        Node::Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, size_t instanceCountReserveSize, Model *model)
        :
            name(name),
            sceneGraph(&sceneGraph),
            bufferManager(&bufferManager),
            parent(parent),
            model(model)
        {
            if (parent != nullptr) {
                parent->addChildNode(this);
            }

            setParallelBufferSizes(instanceCountReserveSize);
            for (size_t i = 0; i < instanceCountReserveSize; ++i) {
                instanceData.instanceIds[i] = getNextInstanceId();
            }
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
            if (parent != nullptr) {
                parent->addChildNode(this);
            }

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
            if (parent != nullptr) {
                parent->addChildNode(this);
            }

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
            return getNodeLocalTransform()
                * instance.translation * instance.rotation * instance.scale;
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

        void Node::setShaderData(RenderSystem::ShaderData newShaderData)
        {
            shaderData = newShaderData;
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
            auto instanceIndexIterator = std::find(instanceData.instanceIds.begin(), instanceData.instanceIds.end(), instanceId);
            if (instanceIndexIterator == instanceData.instanceIds.end()) {
                throw std::runtime_error("Attempted to translate an invalid instanceId");
            }
            auto instanceIndex = std::distance(
                instanceData.instanceIds.begin(),
                instanceIndexIterator
            );
            auto &instance = instanceData.TRSData[instanceIndex];

            instance.translation = glm::translate(
                instance.translation,
                translation
            );
        }

        void Node::rotateInstance(size_t instanceId, float angleRadians, const glm::vec3 &axis)
        {
            auto instanceIndexIterator = std::find(instanceData.instanceIds.begin(), instanceData.instanceIds.end(), instanceId);
            if (instanceIndexIterator == instanceData.instanceIds.end()) {
                throw std::runtime_error("Attempted to rotate an invalid instanceId");
            }
            auto instanceIndex = std::distance(
                instanceData.instanceIds.begin(),
                instanceIndexIterator
            );
            auto &instance = instanceData.TRSData[instanceIndex];

            instance.rotation = glm::rotate(
                instance.rotation,
                angleRadians,
                axis
            );
        }

        void Node::scaleInstance(size_t instanceId, const glm::vec3 &scale)
        {
            auto instanceIndexIterator = std::find(instanceData.instanceIds.begin(), instanceData.instanceIds.end(), instanceId);
            if (instanceIndexIterator == instanceData.instanceIds.end()) {
                throw std::runtime_error("Attempted to scale an invalid instanceId");
            }
            auto instanceIndex = std::distance(
                instanceData.instanceIds.begin(),
                instanceIndexIterator
            );
            auto &instance = instanceData.TRSData[instanceIndex];

            instance.scale = glm::scale(
                instance.scale,
                scale
            );
        }

        void Node::setNodePosition(const glm::vec3 &position)
        {
            TRSData.translation = glm::translate(
                Math::identityMatrix,
                position
            );
        }

        void Node::setNodeRotation(float angleRadians, const glm::vec3 &axis)
        {
            TRSData.rotation = glm::rotate(
                Math::identityMatrix,
                angleRadians,
                axis
            );
        }

        void Node::setNodeScale(const glm::vec3 &scale)
        {
            TRSData.scale = glm::scale(
                Math::identityMatrix,
                scale
            );
        }

        void Node::setInstancePosition(size_t instanceId, const glm::vec3 &position)
        {
            auto instanceIndexIterator = std::find(instanceData.instanceIds.begin(), instanceData.instanceIds.end(), instanceId);
            if (instanceIndexIterator == instanceData.instanceIds.end()) {
                throw std::runtime_error("Attempted to set position on an invalid instanceId");
            }
            auto instanceIndex = std::distance(
                instanceData.instanceIds.begin(),
                instanceIndexIterator
            );
            auto &instance = instanceData.TRSData[instanceIndex];

            instance.translation = glm::translate(
                Math::identityMatrix,
                position
            );
        }

        void Node::setInstanceRotation(size_t instanceId, float angleRadians, const glm::vec3 &axis)
        {
            auto instanceIndexIterator = std::find(instanceData.instanceIds.begin(), instanceData.instanceIds.end(), instanceId);
            if (instanceIndexIterator == instanceData.instanceIds.end()) {
                throw std::runtime_error("Attempted to set rotation on an invalid instanceId");
            }
            auto instanceIndex = std::distance(
                instanceData.instanceIds.begin(),
                instanceIndexIterator
            );
            auto &instance = instanceData.TRSData[instanceIndex];

            instance.rotation = glm::rotate(
                Math::identityMatrix,
                angleRadians,
                axis
            );
        }

        void Node::setInstanceScale(size_t instanceId, const glm::vec3 &scale)
        {
            auto instanceIndexIterator = std::find(instanceData.instanceIds.begin(), instanceData.instanceIds.end(), instanceId);
            if (instanceIndexIterator == instanceData.instanceIds.end()) {
                throw std::runtime_error("Attempted to set scale on an invalid instanceId");
            }
            auto instanceIndex = std::distance(
                instanceData.instanceIds.begin(),
                instanceIndexIterator
            );
            auto &instance = instanceData.TRSData[instanceIndex];

            instance.scale = glm::scale(
                Math::identityMatrix,
                scale
            );
        }

        size_t Node::addInstance()
        {
            auto newInstanceId = getNextInstanceId();

            auto setInstanceToDefaultValues = [&](auto index){
                instanceData.modelMatrices[index] = glm::mat4{};
                instanceData.TRSData[index] = TranslateRotateScaleData{};
                instanceData.instanceIds[index] = newInstanceId;
            };

            if (modelMatrixBuffer == nullptr) {
                setParallelBufferSizes(1);
                instanceData.numActive = 1;
                setInstanceToDefaultValues(0);
                return newInstanceId;
            }

            const auto modelMatrixBufferSize = modelMatrixBuffer->getSizeInElements();

            if (instanceData.numActive < modelMatrixBufferSize) {
                setInstanceToDefaultValues(instanceData.numActive);
                ++instanceData.numActive;
                return newInstanceId;
            }

            const auto newModelMatrixBufferSize = modelMatrixBufferSize * 2;
            setParallelBufferSizes(newModelMatrixBufferSize);
            setInstanceToDefaultValues(instanceData.numActive);
            ++instanceData.numActive;

            return newInstanceId;
        }

        void Node::addChildNode(Node *child)
        {
            if (child->parent != nullptr) {
                child->parent->notifyChildRemoved(child);
            }

            children.push_back(child);
        }

        void Node::removeInstance(size_t instanceId)
        {
            if (instanceData.numActive <= 0) {
                return;
            }

            auto removeIndex = std::distance(
                instanceData.instanceIds.begin(),
                std::find(instanceData.instanceIds.begin(), instanceData.instanceIds.end(), instanceId)
            );
            auto lastActiveInstanceDataIndex = std::min(instanceData.numActive - 1, static_cast<size_t>(0));

            instanceData.instanceIds[removeIndex] = 0;
            std::iter_swap(
                instanceData.modelMatrices.begin() + removeIndex,
                instanceData.modelMatrices.begin() + lastActiveInstanceDataIndex
            );
            std::iter_swap(
                instanceData.TRSData.begin() + removeIndex,
                instanceData.TRSData.begin() + lastActiveInstanceDataIndex
            );
            std::iter_swap(
                instanceData.instanceIds.begin() + removeIndex,
                instanceData.instanceIds.begin() + lastActiveInstanceDataIndex
            );
            --instanceData.numActive;

            if (instanceData.numActive <= 0) {
                modelMatrixBuffer->destroy();
            }
        }

        void Node::removeAllInstances()
        {
            instanceData.modelMatrices.clear();
            instanceData.TRSData.clear();
            instanceData.numActive = 0;
            modelMatrixBuffer->destroy();
        }

        void Node::removeChildNode(Node *removedChild)
        {
            children.erase(
                std::remove_if(
                    children.begin(),
                    children.end(),
                    [&](const auto child){
                        return child->getName() == removedChild->getName();
                    }
                ),
                children.end()
            );
        }

        void Node::removeAllChildren()
        {
            children.clear();
        }

        void Node::render(const glm::mat4 &parentModelMatrix)
        {
            const auto localCoordinateSpace = parentModelMatrix * TRSData.translation * TRSData.rotation * TRSData.scale;

            updateModelMatrixBuffer(localCoordinateSpace);

            if (model != nullptr && modelMatrixBuffer != nullptr && instanceData.numActive > 0) {
                model->render(shaderData, *modelMatrixBuffer, instanceData.numActive);
            }

            for (auto child : children) {
                child->render(localCoordinateSpace);
            }
        }

        void Node::copyModelMatrixBufferOfOtherNode(const Node &other)
        {
            if (modelMatrixBuffer != nullptr) {
                bufferManager->destroy(nodeBaseName + std::to_string(name));
            }

            const auto &otherModelMatrixBuffer = *other.modelMatrixBuffer;
            modelMatrixBuffer = bufferManager->create(
                nodeBaseName + std::to_string(name),
                mvpMatrixBufferName,
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

        size_t Node::getNextInstanceId()
        {
            return nextInstanceIdToAssign++;
        }

        void Node::notifyChildRemoved(Node *removedChild)
        {
            removeChildNode(removedChild);
        }

        void Node::updateModelMatrixBuffer(const glm::mat4 &parentModelMatrix)
        {
            if (instanceData.numActive <= 0) {
                return;
            }

            for (size_t i = 0; i < instanceData.numActive; ++i) {
                instanceData.modelMatrices[i] =
                    parentModelMatrix
                    * instanceData.TRSData[i].translation
                    * instanceData.TRSData[i].rotation
                    * instanceData.TRSData[i].scale;
            }

            modelMatrixBuffer->bufferRange(
                0,
                sizeof(glm::mat4) * instanceData.numActive,
                instanceData.modelMatrices.data()
            );
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
            instanceData.instanceIds.resize(size, 0);
            if (modelMatrixBuffer != nullptr) {
                modelMatrixBuffer = bufferManager->resizeElements(nodeBaseName + std::to_string(name), size);
                return;
            }

            modelMatrixBuffer = bufferManager->create(
                nodeBaseName + std::to_string(name),
                mvpMatrixBufferName,
                getModelMatrixBufferLayout()
            );
            modelMatrixBuffer->allocateBufferStorage(
                sizeof(glm::mat4) * size,
                nullptr,
                getModelMatrixBufferUsageFlags()
            );
        }
    }
}
