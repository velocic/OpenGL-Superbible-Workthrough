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

        void ModelManager::load(const ModelFile &file, std::function<void(Model *)> onLoadComplete)
        {
            auto importer = Assimp::Importer{}; 
            const auto scene = importer.ReadFile(file.path, aiProcess_Triangulate | aiProcess_FlipUVs);

            if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
                throw std::runtime_error(importer.GetErrorString());
            }

            auto modelDirectory = file.path.substr(0, file.path.find_last_of('/'));
            auto meshes = std::vector<std::unique_ptr<Mesh>>{};

            processNode(scene->mRootNode, scene, modelDirectory, meshes);

            auto loadedModel = std::make_unique<Model>(std::move(meshes));
            auto resultForCallback = loadedModel.get();

            models.insert_or_assign(stringHasher(file.alias), std::move(loadedModel));

            onLoadComplete(resultForCallback);
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

                textures = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }

            return std::make_unique<Mesh>(
                std::move(vertices),
                std::move(indices),
                std::move(textures)
            );
        }

        std::vector<std::pair<std::string, RenderSystem::Texture *>> ModelManager::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName, const std::string &modelDirectory)
        {
            auto textureCount = mat->GetTextureCount(type);
            auto loadedTextures = std::vector<std::pair<std::string, RenderSystem::Texture *>>{};
            loadedTextures.reserve(textureCount);

            for (unsigned int i = 0; i < textureCount; ++i) {
                aiString textureName;
                mat->GetTexture(type, i, &textureName);

                //Textures loaded from models will use their full path as their alias, to avoid collisions
                auto textureFullPath = modelDirectory + std::to_string(textureName.C_Str());
                auto loadedTexture = textureManager.get(textureFullPath);

                if (loadedTexture != nullptr) {
                    loadedTextures.emplace_back(textureFullPath, loadedTexture);
                    continue;
                }

                auto fileInfo = RenderSystem::TextureManager::TextureFile{
                    textureFullPath,
                    textureFullPath, //texture's alias name
                    RenderSystem::TextureManager::SupportedFileType::PNG, //TODO: proper detection of file type
                    RS_RGBA //TODO: support for other color channel layouts
                };

                auto initParams = RenderSystem::TextureManager::TextureInitParams{
                    RenderSystem::TextureManager::DEFAULT_NUM_MIPMAP_LEVELS,
                    RS_RGBA8, //TODO: support for other color channel layouts
                    true //auto-generate mipmaps according to number of mip-levels specified
                };

                textureManager.loadTexture2D(
                    fileInfo,
                    initParams,
                    [&loadedTextures, &textureFullPath](auto texture){
                        loadedTextures.emplace_back(textureFullPath, texture);
                    }
                );
            }

            return loadedTextures;
        }

        void ModelManager::batchLoad(const std::vector<ModelFile> &files, std::function<void()> onLoadComplete)
        {
            for (const auto &file : files) {
                load(file, [](auto){});
            }

            onLoadComplete();
        }

        Model *ModelManager::get(const std::string &alias) const
        {
            auto result = models.find(stringHasher(alias));

            if (result != models.end()) {
                return result->second.get();
            }

            return nullptr;
        }

        void ModelManager::remove(const std::string &alias)
        {
            models.erase(stringHasher(alias));
        }

        void ModelManager::removeAll()
        {
            models.clear();
        }
    }
}
