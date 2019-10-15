#ifndef FLARE_SCENEGRAPH_SCENEGRAPH_H
#define FLARE_SCENEGRAPH_SCENEGRAPH_H

#include <unordered_map>

#include <flare/rendersystem/buffermanager.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/scenegraph/model.h>
#include <flare/utility/math.h>

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
                std::unordered_map<size_t, std::unique_ptr<Node>> nodes;
                Node *rootNode = nullptr;
                RenderSystem::BufferManager bufferManager;
                size_t nextNameToAssign = 0;

            public:
                SceneGraph();
                ~SceneGraph() {}
                SceneGraph(SceneGraph &&other);
                SceneGraph &operator=(SceneGraph &&other);
                SceneGraph(const SceneGraph &other) = delete;
                SceneGraph &operator=(const SceneGraph &other) = delete;

                Node *createNode(Node *parent);
                Node *createNode(Node *parent, Model *model);
                Node *createNode(Node *parent, size_t instanceCountReserveSize);
                Node *createNode(Node *parent, size_t instanceCountReserveSize, Model *model);

                void destroyNode(Node *target);
                Node *getRootNode() const;

                void render();

                //Used for fetching a unique numeric id for nodes on creation.
                //Public so that nodes can request this in their copy constructor
                //Avoid using for other scenarios
                size_t requestName();
        };

        class Node
        {
            friend class SceneGraph;
            private:
                struct TranslateRotateScaleData {
                    glm::mat4 translation = Math::identityMatrix;
                    glm::mat4 rotation = Math::identityMatrix;
                    glm::mat4 scale = Math::identityMatrix;
                };

                struct InstanceData {
                    std::vector<glm::mat4> modelMatrices;
                    std::vector<TranslateRotateScaleData> TRSData;
                    size_t numActive = 0;
                };

                const std::string nodeBaseName = "node";
                TranslateRotateScaleData TRSData;
                InstanceData instanceData;
                std::vector<Node *> children;
                size_t name = 0;
                RenderSystem::ShaderData shaderData;
                SceneGraph *sceneGraph = nullptr;
                RenderSystem::BufferManager *bufferManager = nullptr;
                RenderSystem::Buffer *modelMatrixBuffer = nullptr;
                Node *parent = nullptr;
                Model *model = nullptr;

                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent);
                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, Model *model);
                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, size_t instanceCountReserveSize);
                Node(SceneGraph &sceneGraph, RenderSystem::BufferManager &bufferManager, size_t name, Node *parent, size_t instanceCountReserveSize, Model *model);

                void copyModelMatrixBufferOfOtherNode(const Node &other);
                void deepCopyChildrenOfOtherNode(std::vector<Node *> &destination, const Node &other);
                void notifyChildRemoved(Node *child);
                RenderSystem::VertexDataLayout getModelMatrixBufferLayout() const;
                constexpr RenderSystem::RSbitfield getModelMatrixBufferUsageFlags() const {
                    return RenderSystem::RSbitfield{} | RenderSystem::RS_DYNAMIC_STORAGE_BIT;
                }
                void setParallelBufferSizes(size_t size);
            public:
                ~Node();
                Node(const Node &other);
                Node(Node &&other);
                Node &operator=(const Node &other);
                Node &operator=(Node &&other);

                void destroy();

                size_t getName() const;
                glm::mat4 getNodeLocalTransform() const;
                glm::mat4 getInstanceLocalTransform(size_t instanceId) const;

                void setParent(Node *newParent);
                void setModel(Model *newModel);
                void setShaderData(RenderSystem::ShaderData newShaderData);

                void translateNode(const glm::vec3 &translation);
                void rotateNode(float angleRadians, const glm::vec3 &axis);
                void scaleNode(const glm::vec3 &scale);

                void translateInstance(size_t instanceId, const glm::vec3 &translation);
                void rotateInstance(size_t instanceId, float angleRadians, const glm::vec3 &axis);
                void scaleInstance(size_t instanceId, const glm::vec3 &scale);

                size_t addInstance();
                void addChildNode(Node *child);
                void removeInstance(size_t instanceId);
                void removeAllInstances();
                void removeChildNode(Node *child);
                void removeAllChildren();

                void render();
        };
    }
}

#endif
