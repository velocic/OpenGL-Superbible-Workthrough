#ifndef FLARE_SCENEGRAPH_NODE_H
#define FLARE_SCENEGRAPH_NODE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <flare/rendersystem/resizablebuffer.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/scenegraph/model.h>
#include <flare/utility/datatypes.h>
#include <flare/utility/math.h>

#include <glm-0.9.9/glm.hpp>
#include <glm-0.9.9/gtc/matrix_transform.hpp>
#include <glm-0.9.9/gtx/matrix_decompose.hpp>

namespace Flare
{
    namespace SceneGraph
    {
        class SceneGraph;
        class Node
        {
            friend class DirectRenderedSceneGraph;
            friend class IndirectRenderedSceneGraph;

            private:
                struct TranslateRotateScaleData {
                    glm::mat4 translation = Math::identityMatrix;
                    glm::mat4 rotation = Math::identityMatrix;
                    glm::mat4 scale = Math::identityMatrix;
                };

                struct InstanceData {
                    std::vector<glm::mat4> modelMatrices;
                    std::vector<TranslateRotateScaleData> TRSData;
                    std::unordered_map<size_t, size_t> instanceIdLookupTable;
                    size_t numActive = 0;
                };

                size_t nextInstanceIdToAssign = 0;
                const std::string nodeBaseName = "node";
                const std::string mvpMatrixBufferName = "mvpMatrix";
                TranslateRotateScaleData TRSData;
                InstanceData instanceData;
                std::vector<std::unique_ptr<Node>> children;
                size_t name = 0;
                RenderSystem::ShaderData shaderData;
                std::vector<const RenderSystem::Buffer *> userProvidedShaderBuffers;
                RenderSystem::ResizableBuffer modelMatrixBuffer;
                SceneGraph *sceneGraph = nullptr;
                Node *parent = nullptr;
                Model *model = nullptr;

                Node(SceneGraph &sceneGraph, size_t name, Node *parent);
                Node(SceneGraph &sceneGraph, size_t name, Node *parent, Model *model);
                Node(SceneGraph &sceneGraph, size_t name, Node *parent, size_t instanceCountReserveSize);
                Node(SceneGraph &sceneGraph, size_t name, Node *parent, size_t instanceCountReserveSize, Model *model);

                void copyModelMatrixBufferOfOtherNode(const Node &other);
                void deepCopyChildrenOfOtherNode(std::vector<std::unique_ptr<Node>> &destination, const Node &other);
                size_t getNextInstanceId();
                void updateModelMatrixBuffer(const glm::mat4 &parentModelMatrix, const std::vector<glm::mat4> &submeshLocalTransforms);
                RenderSystem::VertexDataLayout getModelMatrixBufferLayout() const;
                constexpr RenderSystem::RSbitfield getModelMatrixBufferUsageFlags() const {
                    return RenderSystem::RSbitfield{} | RenderSystem::RS_DYNAMIC_STORAGE_BIT;
                }
                void setParallelBufferSizes(size_t size);
            public:
                ~Node() {};
                Node(Node &&other);
                Node &operator=(Node &&other);
                Node(const Node &other) = delete;
                Node &operator=(const Node &other) = delete;

                size_t addInstance();
                void addChildNode(Node *child);
                Node *createChildNode();
                Node *copy() const;

                DataTypes::DecomposedModelMatrix decomposeInstance(size_t instanceId) const;
                DataTypes::DecomposedModelMatrix decomposeNode() const;
                void destroy();

                size_t getName() const;
                glm::mat4 getNodeLocalTransform() const;
                glm::mat4 getInstanceLocalTransform(size_t instanceId) const;

                void setModel(Model *newModel);
                void setShaderData(RenderSystem::ShaderData newShaderData);
                void setShaderBuffers(const std::vector<const RenderSystem::Buffer *> &buffers);

                void translateNode(const glm::vec3 &translation);
                void rotateNode(float angleRadians, const glm::vec3 &axis);
                void scaleNode(const glm::vec3 &scale);

                void translateInstance(size_t instanceId, const glm::vec3 &translation);
                void rotateInstance(size_t instanceId, float angleRadians, const glm::vec3 &axis);
                void scaleInstance(size_t instanceId, const glm::vec3 &scale);

                void setNodePosition(const glm::vec3 &position);
                void setNodeRotation(float angleRadians, const glm::vec3 &axis);
                void setNodeScale(const glm::vec3 &scale);

                void setInstancePosition(size_t instanceId, const glm::vec3 &position);
                void setInstanceRotation(size_t instanceId, float angleRadians, const glm::vec3 &axis);
                void setInstanceScale(size_t instanceId, const glm::vec3 &scale);

                void removeInstance(size_t instanceId);
                void removeAllInstances();
                void removeAllChildren();

                void render(const glm::mat4 &parentModelMatrix);
                std::vector<Mesh::SortableDrawElementsIndirectCommand> getIndirectDrawCommands(const glm::mat4 &parentModelMatrix);
        };
    }
}

#endif
