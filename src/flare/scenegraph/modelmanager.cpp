#include <flare/scenegraph/modelmanager.h>

namespace Flare
{
    namespace SceneGraph
    {
        ModelManager::ModelManager(RenderSystem::TextureManager &textureManager)
        :
            textureManager(textureManager)
        {
        }

        ModelManager::~ModelManager()
        {
            removeAll();
        }

        ModelManager::ModelManager(ModelManager &&other)
        :
            textureManager(other.textureManager),
            models(std::move(other.models))
        {
        }

        ModelManager &ModelManager::operator=(ModelManager &&other)
        {
            textureManager = other.textureManager;
            models = std::move(other.models);

            return *this;
        }

        void ModelManager::loadModel(const std::string &filePath)
        {
            auto importer = Assimp::Importer{}; 
            const auto scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

            if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
                throw std::runtime_error(importer.GetErrorString());
            }

            auto modelDirectory = filePath.substr(0, filePath.find_last_of('/'));
            auto meshes = std::vector<std::unique_ptr<Mesh>>{};

            processNode(scene->mRootNode, scene, modelDirectory, meshes);
        }

        void ModelManager::processNode(aiNode *node, const aiScene *scene, const std::string &modelDirectory, std::vector<std::unique_ptr<Mesh>> &meshes)
        {
            //Process all meshes attached to this node
            for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
                auto mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(
                    processMesh(mesh, scene)
                );
            }

            //Process all nodes of this node's children
            for (unsigned int i = 0; i < node->mNumChildren; ++i) {
                processNode(node->mChildren[i], scene);
            }
        }

        std::unique_ptr<Mesh> ModelManager::processMesh(aiMesh *mesh, const aiScene *scene, const std::string &modelDirectory)
        {
            std::vector<DataTypes::Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<std::pair<std::string, RenderSystem::Texture *>> textures;

            vertices.resize(mesh->mNumVertices);
            indices.reserve(mesh->mNumFaces * 3);

            //process vertex positions, normals, tex coordinates
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                auto &vertex = vertices[i];
                const auto &sourceVertex = mesh->mVertices[i];
                const auto &sourceNormal = mesh->mNormals[i];

                vertex.position.x = sourceVertex.x;
                vertex.position.y = sourceVertex.y;
                vertex.position.z = sourceVertex.z;

                vertex.normal.x = sourceNormal.x;
                vertex.normal.y = sourceNormal.y;
                vertex.normal.z = sourceNormal.z;

                if (mesh->mTextureCoords[0] == nullptr) {
                    vertex.uvCoords.x = 0.0f;
                    vertex.uvCoords.y = 0.0f;

                    continue;
                }

                const auto &sourceUVs = mesh->mTextureCoords[0][i];

                vertex.uvCoords.x = sourceUVs.x;
                vertex.uvCoords.y = sourceUVs.y;
            }

            //process indices
            for (const auto &face : mesh->mFaces) {
                for (const auto &index : face.mIndices) {
                    indices.push_back(index);
                }
            }

            //process material
            if (mesh->mMaterialIndex >= 0) {
                auto material = scene->mMaterials[mesh->mMaterialIndex];

                auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

                //TODO: insert textures somewhere
            }

            //construct mesh (register with mesh manager?
            return std::make_unique<Mesh>(
                std::move(vertices),
                std::move(indices),
                std::move(textures)
            );
        }

        std::vector<std::pair<std::string, RenderSystem::Texture *>> ModelManager::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
        {
        }

        void ModelManager::batchLoad(const std::vector<ModelFile> &files, std::function<void()> onLoadComplete)
        {
            //TODO: loop invoke load();

            onLoadComplete();
        }

        void ModelManager::load(const ModelFile &file, std::function<void(Model *)> onLoadComplete)
        {
        }

        Model *ModelManager::get(const std::string &alias) const
        {
        }

        void ModelManager::remove(const std::string &alias)
        {
        }

        void ModelManager::removeAll()
        {
        }
    }
}
