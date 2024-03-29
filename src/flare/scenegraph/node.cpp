#include <flare/scenegraph/node.h>

#include <flare/scenegraph/scenegraph.h>

namespace Flare
{
    namespace SceneGraph
    {
        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent)
        {
            setParallelBufferSizes(1);
            instanceData.instanceIdLookupTable[getNextInstanceId()] = 0;
        }

        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent, Model *model)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent),
            model(model)
        {
            setParallelBufferSizes(1);
            instanceData.instanceIdLookupTable[getNextInstanceId()] = 0;
        }

        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent, size_t instanceCountReserveSize)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent)
        {
            setParallelBufferSizes(instanceCountReserveSize);
            for (size_t i = 0; i < instanceCountReserveSize; ++i) {
                instanceData.instanceIdLookupTable[getNextInstanceId()] = i;
            }
        }

        Node::Node(SceneGraph &sceneGraph, size_t name, Node *parent, size_t instanceCountReserveSize, Model *model)
        :
            name(name),
            sceneGraph(&sceneGraph),
            parent(parent),
            model(model)
        {
            setParallelBufferSizes(instanceCountReserveSize);
            for (size_t i = 0; i < instanceCountReserveSize; ++i) {
                instanceData.instanceIdLookupTable[getNextInstanceId()] = i;
            }
        }

        Node::Node(Node &&other)
        :
            nextInstanceIdToAssign(std::exchange(other.nextInstanceIdToAssign, 0)),
            TRSData(std::move(other.TRSData)),
            instanceData(std::move(other.instanceData)),
            children(std::move(other.children)),
            name(std::exchange(other.name, 0)),
            shaderData(std::move(other.shaderData)),
            userProvidedShaderBuffers(std::move(other.userProvidedShaderBuffers)),
            modelMatrixBuffer(std::move(other.modelMatrixBuffer)),
            sceneGraph(std::exchange(other.sceneGraph, nullptr)),
            parent(std::exchange(other.parent, nullptr)),
            model(std::exchange(other.model, nullptr))
        {
        }

        Node &Node::operator=(Node &&other)
        {
            nextInstanceIdToAssign = std::exchange(other.nextInstanceIdToAssign, 0);
            TRSData = std::move(other.TRSData);
            instanceData = std::move(other.instanceData);
            children = std::move(other.children);
            name = std::exchange(other.name, 0);
            shaderData = std::move(other.shaderData);
            userProvidedShaderBuffers = std::move(other.userProvidedShaderBuffers);
            modelMatrixBuffer = std::move(other.modelMatrixBuffer);
            sceneGraph = std::exchange(other.sceneGraph, nullptr);
            parent = std::exchange(other.parent, nullptr);
            model = std::exchange(other.model, nullptr);

            return *this;
        }

        DataTypes::DecomposedModelMatrix Node::decomposeInstance(size_t instanceId) const
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to decompose an invalid instanceId");
            }

            auto result = DataTypes::DecomposedModelMatrix{};
            glm::decompose(
                getInstanceLocalTransform(instanceId),
                result.scale,
                result.orientation,
                result.translation,
                result.skew,
                result.perspective
            );

            return result;
        }

        DataTypes::DecomposedModelMatrix Node::decomposeNode() const
        {
            auto result = DataTypes::DecomposedModelMatrix{};

            glm::decompose(
                getNodeLocalTransform(),
                result.scale,
                result.orientation,
                result.translation,
                result.skew,
                result.perspective
            );

            return result;
        }

        void Node::destroy()
        {
            for (auto &child : children) {
                child->destroy();
            }

            //Delete this instance's backing data (which is owned by this instance's parent)
            if (parent != nullptr) {
                auto thisInstancesOwningPointerIterator = std::find_if(
                    parent->children.begin(),
                    parent->children.end(),
                    [&](const auto &parentsChild){
                        return parentsChild->getName() == getName();
                    }
                );
                if (thisInstancesOwningPointerIterator != parent->children.end()) {
                    parent->children.erase(thisInstancesOwningPointerIterator);
                }
            }
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

        void Node::setModel(Model *newModel)
        {
            model = newModel;

            //Resize the modelMatrix buffer if the new model contains more than 1 mesh
            if (model->getMeshCount() != 1) {
                setParallelBufferSizes(std::max(instanceData.numActive, size_t{1}));
            }
        }

        void Node::setShaderData(RenderSystem::ShaderData newShaderData)
        {
            shaderData = newShaderData;
        }

        void Node::setShaderBuffers(const std::vector<const RenderSystem::Buffer *> &buffers)
        {
            userProvidedShaderBuffers = buffers;
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
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to translate an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.translation = glm::translate(
                instance.translation,
                translation
            );
        }

        void Node::rotateInstance(size_t instanceId, float angleRadians, const glm::vec3 &axis)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to rotate an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.rotation = glm::rotate(
                instance.rotation,
                angleRadians,
                axis
            );
        }

        void Node::scaleInstance(size_t instanceId, const glm::vec3 &scale)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to scale an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

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
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to set position on an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.translation = glm::translate(
                Math::identityMatrix,
                position
            );
        }

        void Node::setInstanceRotation(size_t instanceId, float angleRadians, const glm::vec3 &axis)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to set rotation on an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.rotation = glm::rotate(
                Math::identityMatrix,
                angleRadians,
                axis
            );
        }

        void Node::setInstanceScale(size_t instanceId, const glm::vec3 &scale)
        {
            auto instanceIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (instanceIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to set scale on an invalid instanceId");
            }
            auto &instance = instanceData.TRSData[instanceIndexIterator->second];

            instance.scale = glm::scale(
                Math::identityMatrix,
                scale
            );
        }

        size_t Node::addInstance()
        {
            auto newInstanceId = getNextInstanceId();

            auto setInstanceToDefaultValues = [&](auto index){
                instanceData.TRSData[index] = TranslateRotateScaleData{};
                instanceData.instanceIdLookupTable[newInstanceId] = index;
            };

            if (modelMatrixBuffer.get() == nullptr) {
                setParallelBufferSizes(1);
                instanceData.numActive = 1;
                setInstanceToDefaultValues(0);
                return newInstanceId;
            }

            const auto modelMatrixBufferSize = modelMatrixBuffer.get()->getSizeInElements();

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

        Node *Node::createChildNode()
        {
            auto newChild = std::unique_ptr<Node>(new Node(*sceneGraph, sceneGraph->requestName(), this));
            auto result = newChild.get();
            children.push_back(std::move(newChild));

            return result;
        }

        void Node::addChildNode(Node *child)
        {
            if (child->parent == nullptr) {
                throw std::runtime_error("Attempted to re-parent the root scene node (which is disallowed) or the passed argument has no parent (which is only allowed for the root scene node)");
            }

            if (child->getName() == getName()) {
                throw std::runtime_error("Attempted to re-parent a scene node to itself");
            }

            if (child->parent->getName() == getName()) {
                //Attempting to reparent to the same parent; nothing to do.
                return;
            }

            auto &oldParentNode = *child->parent;
            auto childOwningPointerIterator = std::find_if(
                oldParentNode.children.begin(),
                oldParentNode.children.end(),
                [&](const auto &oldParentsChild){
                    return oldParentsChild->getName() == child->getName();
                }
            );

            child->parent = this;
            children.push_back(std::move(*childOwningPointerIterator));
            oldParentNode.children.erase(childOwningPointerIterator);
        }

        Node *Node::copy() const
        {
            if (parent == nullptr) {
                throw std::runtime_error("Attempted to copy the root scene node (which is disallowed)");
            }

            auto copyNode = std::unique_ptr<Node>(new Node(*sceneGraph, sceneGraph->requestName(), parent));
            copyNode->TRSData = TRSData;
            copyNode->instanceData = instanceData;
            copyNode->model = model;
            copyNode->shaderData = shaderData;
            copyNode->userProvidedShaderBuffers = userProvidedShaderBuffers;
            copyNode->deepCopyChildrenOfOtherNode(copyNode->children, *this);
            copyNode->copyModelMatrixBufferOfOtherNode(*this);

            auto result = copyNode.get();
            parent->children.push_back(std::move(copyNode));

            return result;
        }

        void Node::removeInstance(size_t instanceId)
        {
            if (instanceData.numActive == 0) {
                return;
            }

            auto removeIndexIterator = instanceData.instanceIdLookupTable.find(instanceId);
            if (removeIndexIterator == instanceData.instanceIdLookupTable.end()) {
                throw std::runtime_error("Attempted to remove an invalid instanceId");
            }
            auto removeIndex = removeIndexIterator->second;

            auto lastActiveInstanceDataIndex = instanceData.numActive - 1;

            std::iter_swap(
                instanceData.TRSData.begin() + removeIndex,
                instanceData.TRSData.begin() + lastActiveInstanceDataIndex
            );
            instanceData.instanceIdLookupTable.erase(instanceId);
            --instanceData.numActive;

            if (instanceData.numActive == 0) {
                modelMatrixBuffer.destroy();
            }
        }

        void Node::removeAllInstances()
        {
            instanceData.modelMatrices.clear();
            instanceData.TRSData.clear();
            instanceData.numActive = 0;
            modelMatrixBuffer.destroy();
        }

        void Node::removeAllChildren()
        {
            children.clear();
        }

        void Node::render(const glm::mat4 &parentModelMatrix)
        {
            const auto localCoordinateSpace = parentModelMatrix * TRSData.translation * TRSData.rotation * TRSData.scale;

            if (model != nullptr && modelMatrixBuffer.get() != nullptr && instanceData.numActive > 0) {
                updateModelMatrixBuffer(localCoordinateSpace, model->getMeshLocalTransforms());

                model->render(shaderData, *modelMatrixBuffer.get(), userProvidedShaderBuffers, instanceData.numActive);
            }

            for (auto &child : children) {
                child->render(localCoordinateSpace);
            }
        }

        std::vector<Mesh::SortableDrawElementsIndirectCommand> Node::getIndirectDrawCommands(const glm::mat4 &parentModelMatrix)
        {
            const auto localCoordinateSpace = parentModelMatrix * TRSData.translation * TRSData.rotation * TRSData.scale;

            auto accumulatedResults = std::vector<Mesh::SortableDrawElementsIndirectCommand>{};

            if (model != nullptr && modelMatrixBuffer.get() != nullptr && instanceData.numActive > 0) {
                updateModelMatrixBuffer(localCoordinateSpace, model->getMeshLocalTransforms());
                auto modelDrawCommands = model->getIndirectDrawCommands(shaderData, *modelMatrixBuffer.get(), userProvidedShaderBuffers, instanceData.numActive);
                std::move(modelDrawCommands.begin(), modelDrawCommands.end(), std::back_inserter(accumulatedResults));
            }

            for (auto &child : children) {
                auto childAccumulatedDrawCommands = child->getIndirectDrawCommands(localCoordinateSpace);
                std::move(childAccumulatedDrawCommands.begin(), childAccumulatedDrawCommands.end(), std::back_inserter(accumulatedResults));
            }

            return accumulatedResults;
        }

        void Node::copyModelMatrixBufferOfOtherNode(const Node &other)
        {
            if (modelMatrixBuffer.get() != nullptr) {
                modelMatrixBuffer.destroy();
            }

            modelMatrixBuffer.create(
                mvpMatrixBufferName,
                other.modelMatrixBuffer.get()->getContentDescription()
            );
            modelMatrixBuffer.get()->allocateBufferStorage(
                other.modelMatrixBuffer.get()->getSizeInBytes(),
                nullptr,
                other.modelMatrixBuffer.get()->getUsageFlags()
            );
            modelMatrixBuffer.get()->copyRange(
                *other.modelMatrixBuffer.get(),
                0,
                0,
                other.modelMatrixBuffer.get()->getSizeInBytes()
            );
        }

        void Node::deepCopyChildrenOfOtherNode(std::vector<std::unique_ptr<Node>> &destination, const Node &other)
        {
            for (const auto &child : other.children) {
                auto newNode = std::unique_ptr<Node>(new Node(*sceneGraph, sceneGraph->requestName(), parent));
                newNode->TRSData = child->TRSData;
                newNode->instanceData = child->instanceData;
                newNode->model = child->model;
                newNode->shaderData = child->shaderData;
                newNode->userProvidedShaderBuffers = child->userProvidedShaderBuffers;
                newNode->copyModelMatrixBufferOfOtherNode(*child);
                destination.push_back(std::move(newNode));
                deepCopyChildrenOfOtherNode(newNode->children, *child);
            }
        }

        size_t Node::getNextInstanceId()
        {
            return nextInstanceIdToAssign++;
        }

        void Node::updateModelMatrixBuffer(const glm::mat4 &parentModelMatrix, const std::vector<glm::mat4> &subMeshLocalTransforms)
        {
            if (instanceData.numActive <= 0) {
                return;
            }

            for (size_t subMeshIndex = 0; subMeshIndex < subMeshLocalTransforms.size(); ++subMeshIndex) {
                const auto &subMeshLocalTransform = subMeshLocalTransforms[subMeshIndex];

                for (size_t instanceDataBaseIndex = 0; instanceDataBaseIndex < instanceData.numActive; ++instanceDataBaseIndex) {
                    auto bufferOutputIndex = instanceDataBaseIndex + instanceData.numActive * subMeshIndex;

                    instanceData.modelMatrices[bufferOutputIndex] =
                        parentModelMatrix
                        * instanceData.TRSData[instanceDataBaseIndex].translation
                        * instanceData.TRSData[instanceDataBaseIndex].rotation
                        * instanceData.TRSData[instanceDataBaseIndex].scale
                        * subMeshLocalTransform;
                }
            }

            modelMatrixBuffer.get()->bufferRange(
                0,
                sizeof(glm::mat4) * instanceData.modelMatrices.size(),
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
            instanceData.TRSData.resize(size);

            //Create room for the model matrices for all submeshes in a model (which probably have their own local transforms
            //relative to the model's origin)
            auto modelMatrixBufferSize = size;
            if (model != nullptr) {
                modelMatrixBufferSize = size * model->getMeshCount();
            }

            instanceData.modelMatrices.resize(modelMatrixBufferSize);

            if (modelMatrixBuffer.get() != nullptr) {
                modelMatrixBuffer.resizeElements(modelMatrixBufferSize);
                return;
            }

            modelMatrixBuffer.create(
                mvpMatrixBufferName,
                getModelMatrixBufferLayout()
            )->allocateBufferStorage(
                sizeof(glm::mat4) * modelMatrixBufferSize,
                nullptr,
                getModelMatrixBufferUsageFlags()
            );
        }
    }
}
