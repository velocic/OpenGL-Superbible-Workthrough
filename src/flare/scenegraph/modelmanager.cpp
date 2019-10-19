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
            auto alreadyLoadedModel = get(file.alias);
            if (alreadyLoadedModel != nullptr) {
                onLoadComplete(alreadyLoadedModel);
                return;
            }

            auto importer = Assimp::Importer{};
            const auto scene = importer.ReadFile(file.path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

            if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
                throw std::runtime_error(importer.GetErrorString());
            }

            auto lastSlashIndex = file.path.rfind('/');
            auto modelDirectory = file.path.substr(0, lastSlashIndex + 1);
            auto modelName = file.path.substr(lastSlashIndex + 1, file.path.rfind('.') - lastSlashIndex - 1);
            auto meshes = std::vector<std::unique_ptr<Mesh>>{};

            processNode(scene->mRootNode, scene, modelDirectory, modelName, meshes);

            auto loadedModel = std::make_unique<Model>(std::move(meshes));
            auto resultForCallback = loadedModel.get();

            models.insert_or_assign(stringHasher(file.alias), std::move(loadedModel));

            onLoadComplete(resultForCallback);
        }

        void ModelManager::load(const std::string &alias, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PBRMaterialTextures &textures, std::function<void(Model *)> onLoadComplete)
        {
            throw std::runtime_error("Models using PBR materials are not yet supported by ModelManager");
        }

        void ModelManager::load(const std::string &alias, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PhongMaterialTextures &textures, std::function<void(Model *)> onLoadComplete)
        {
            auto alreadyLoadedModel = get(alias);
            if (alreadyLoadedModel != nullptr) {
                onLoadComplete(alreadyLoadedModel);
                return;
            }

            auto modelMeshes = std::vector<std::unique_ptr<Mesh>>{};
            modelMeshes.emplace_back(std::make_unique<BasicMesh>(std::move(vertices), std::move(indices), textures));

            auto model = std::make_unique<Model>(std::move(modelMeshes));
            auto result = model.get();
            models.insert_or_assign(stringHasher(alias), std::move(model));

            onLoadComplete(result);
        }

        void ModelManager::load(const std::string &alias, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::function<void(Model *)> onLoadComplete)
        {
            auto alreadyLoadedModel = get(alias);
            if (alreadyLoadedModel != nullptr) {
                onLoadComplete(alreadyLoadedModel);
                return;
            }

            auto modelMeshes = std::vector<std::unique_ptr<Mesh>>{};
            modelMeshes.emplace_back(std::make_unique<BasicMesh>(std::move(vertices), std::move(indices)));

            auto model = std::make_unique<Model>(std::move(modelMeshes));
            auto result = model.get();
            models.insert_or_assign(stringHasher(alias), std::move(model));

            onLoadComplete(result);
        }

        void ModelManager::loadWithPackedSubMeshes(const ModelFile &file, std::function<void(Model *)> onLoadComplete)
        {
            auto alreadyLoadedModel = get(file.alias);
            if (alreadyLoadedModel != nullptr) {
                onLoadComplete(alreadyLoadedModel);
                return;
            }

            auto importer = Assimp::Importer{};
            const auto scene = importer.ReadFile(file.path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

            if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
                throw std::runtime_error(importer.GetErrorString());
            }

            auto lastSlashIndex = file.path.rfind('/');
            auto modelDirectory = file.path.substr(0, lastSlashIndex + 1);
            auto modelName = file.path.substr(lastSlashIndex + 1, file.path.rfind('.') - lastSlashIndex - 1);

            auto packedSubMeshData = PackedSubMeshes{};
            processNode(scene->mRootNode, scene, modelDirectory, modelName, packedSubMeshData);

            auto meshForModel = std::vector<std::unique_ptr<Mesh>>{};
            meshForModel.push_back(
                std::make_unique<PackedMesh>(
                    std::move(packedSubMeshData.vertices),
                    std::move(packedSubMeshData.indices),
                    std::move(packedSubMeshData.subMeshEntries)
                )
            );
            auto loadedModel = std::make_unique<Model>(std::move(meshForModel));
            auto resultForCallback = loadedModel.get();

            models.insert_or_assign(stringHasher(file.alias), std::move(loadedModel));

            onLoadComplete(resultForCallback);
        }

        void ModelManager::processNode(aiNode *node, const aiScene *scene, const std::string &modelDirectory, const std::string &modelName, std::vector<std::unique_ptr<Mesh>> &meshes)
        {
            for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
                auto mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(
                    processMesh(mesh, scene, modelDirectory, modelName)
                );
            }

            for (unsigned int i = 0; i < node->mNumChildren; ++i) {
                processNode(node->mChildren[i], scene, modelDirectory, modelName, meshes);
            }
        }

        std::unique_ptr<Mesh> ModelManager::processMesh(aiMesh *mesh, const aiScene *scene, const std::string &modelDirectory, const std::string &modelName)
        {
            std::vector<DataTypes::Vertex> vertices;
            std::vector<unsigned int> indices;

            vertices.resize(mesh->mNumVertices);
            indices.reserve(mesh->mNumFaces * 3);

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

            for (unsigned int currentFace = 0; currentFace < mesh->mNumFaces; ++currentFace) {
                const auto &face = mesh->mFaces[currentFace];
                for (unsigned int index = 0; index < face.mNumIndices; ++index) {
                    indices.push_back(face.mIndices[index]);
                }
            }

            auto material = scene->mMaterials[mesh->mMaterialIndex];

            return std::make_unique<BasicMesh>(
                std::move(vertices),
                std::move(indices),
                loadPhongMaterialTextures(material, modelDirectory, modelName)
            );
        }

        void ModelManager::processNode(aiNode *node, const aiScene *scene, const std::string &modelDirectory, const std::string &modelName, PackedSubMeshes &packedMeshes)
        {
            for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
                auto mesh = scene->mMeshes[node->mMeshes[i]];
                processMesh(mesh, scene, modelDirectory, modelName, packedMeshes);
            }

            for (unsigned int i = 0; i < node->mNumChildren; ++i) {
                processNode(node->mChildren[i], scene, modelDirectory, modelName, packedMeshes);
            }
        }

        void ModelManager::processMesh(aiMesh *mesh, const aiScene *scene, const std::string &modelDirectory, const std::string &modelName, PackedSubMeshes &packedMeshes)
        {
            std::vector<DataTypes::Vertex> vertices;
            std::vector<unsigned int> indices;

            vertices.resize(mesh->mNumVertices);
            indices.reserve(mesh->mNumFaces * 3);

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

            for (unsigned int currentFace = 0; currentFace < mesh->mNumFaces; ++currentFace) {
                const auto &face = mesh->mFaces[currentFace];
                for (unsigned int index = 0; index < face.mNumIndices; ++index) {
                    indices.push_back(face.mIndices[index]);
                }
            }

            auto material = scene->mMaterials[mesh->mMaterialIndex];

            auto subMeshEntry = PackedMesh::SubMeshEntry{};
            subMeshEntry.elementCount = indices.size();
            subMeshEntry.elementBufferOffset = packedMeshes.indices.size() * sizeof(RenderSystem::RSsizei);
            subMeshEntry.baseVertex = packedMeshes.vertices.size();
            subMeshEntry.textures = loadPhongMaterialTextures(material, modelDirectory, modelName);

            packedMeshes.vertices.insert(
                packedMeshes.vertices.end(),
                vertices.begin(),
                vertices.end()
            );
            packedMeshes.indices.insert(
                packedMeshes.indices.end(),
                indices.begin(),
                indices.end()
            );
            packedMeshes.subMeshEntries.push_back(std::move(subMeshEntry));
        }

        RenderSystem::PhongMaterialTextures ModelManager::loadPhongMaterialTextures(aiMaterial *mat, const std::string &modelDirectory, const std::string &modelName)
        {
            auto textureAlias = modelDirectory + modelName;
            if (textureManager.arePhongMaterialTexturesLoaded(textureAlias)) {
                return textureManager.getPhongMaterialTextures(textureAlias);
            }

            auto loadTexturesOfType = [&](
                auto &textureManager,
                const auto &modelDirectory,
                const auto &textureAlias,
                const auto assimpMaterial,
                auto assimpTexType
            ) {
                auto textureCount = assimpMaterial->GetTextureCount(assimpTexType);
                auto textureLoadTargets = std::vector<RenderSystem::TextureManager::PhongTextureFile>{};
                textureLoadTargets.reserve(textureCount);

                auto textureInitParams = RenderSystem::TextureManager::TextureInitParams{
                    RenderSystem::TextureManager::DEFAULT_NUM_MIPMAP_LEVELS,
                    RenderSystem::RS_RGBA8, //TODO: support for other color channel layouts
                    true //auto-generate mipmaps according to number of mip-levels specified
                };

                for (size_t i = 0; i < textureCount; ++i) {
                    auto textureName = aiString{};
                    assimpMaterial->GetTexture(assimpTexType, i, &textureName);

                    //Textures loaded from models will use their model path + model name (minus extension) as their lookup key
                    auto textureFullPath = modelDirectory + std::string{textureName.C_Str()};

                    textureLoadTargets.push_back(
                        RenderSystem::TextureManager::PhongTextureFile{
                            textureFullPath,
                            textureAlias,
                            aiTexTypeToPhongTexType(assimpTexType),
                            RenderSystem::TextureManager::SupportedFileType::PNG, //TODO: proper detection of file type
                            RenderSystem::RS_RGBA //TODO: support for other color channel layouts
                        }
                    );
                }

                textureManager.batchLoadTexture2D(
                    textureLoadTargets,
                    textureInitParams,
                    [](){}
                );
            };

            loadTexturesOfType(textureManager, modelDirectory, textureAlias, mat, aiTextureType_DIFFUSE);
            loadTexturesOfType(textureManager, modelDirectory, textureAlias, mat, aiTextureType_SPECULAR);
            loadTexturesOfType(textureManager, modelDirectory, textureAlias, mat, aiTextureType_NORMALS);

            return textureManager.getPhongMaterialTextures(textureAlias);
        }

        RenderSystem::TextureManager::PhongMaterialTextureType ModelManager::aiTexTypeToPhongTexType(aiTextureType aiTexType)
        {
            if (aiTexType == aiTextureType_DIFFUSE) {
                return RenderSystem::TextureManager::PhongMaterialTextureType::DIFFUSE;
            } else if (aiTexType == aiTextureType_SPECULAR) {
                return RenderSystem::TextureManager::PhongMaterialTextureType::SPECULAR;
            }

            return RenderSystem::TextureManager::PhongMaterialTextureType::NORMAL;
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
