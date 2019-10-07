#ifndef FLARE_SCENEGRAPH_MODELMANAGER_H
#define FLARE_SCENEGRAPH_MODELMANAGER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <flare/scenegraph/model.h>
#include <flare/rendersystem/texturemanager.h>

namespace Flare
{
    namespace SceneGraph
    {
        class ModelManager
        {
            private:
                RenderSystem::TextureManager &textureManager;
                std::unordered_map<size_t, std::unique_ptr<Model>> models;
                std::hash<std::string> stringHasher;

                void processNode(aiNode *node, const aiScene *scene, const std::string &modelDirectory, const std::string &modelName, std::vector<std::unique_ptr<BasicMesh>> &meshes);
                std::unique_ptr<BasicMesh> processMesh(aiMesh *mesh, const aiScene *scene, const std::string &modelDirectory, const std::string &modelName);
                RenderSystem::PhongMaterialTextures loadPhongMaterialTextures(aiMaterial *mat, const std::string &modelDirectory, const std::string &modelName);
                RenderSystem::TextureManager::PhongMaterialTextureType aiTexTypeToPhongTexType(aiTextureType aiTexType);
            public:
                struct ModelFile
                {
                    std::string alias;
                    std::string path;
                };

                ModelManager(RenderSystem::TextureManager &textureManager);
                ~ModelManager();
                ModelManager(ModelManager &&other);
                ModelManager &operator=(ModelManager &&other);
                ModelManager(const ModelManager &other) = delete;
                ModelManager &operator=(const ModelManager &other) = delete;

                void batchLoad(const std::vector<ModelFile> &files, std::function<void()> onLoadComplete);
                void load(const ModelFile &file, std::function<void(Model *)> onLoadComplete);
                void load(const std::string &alias, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PBRMaterialTextures &textures, std::function<void(Model *)> onLoadComplete);
                void load(const std::string &alias, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, const RenderSystem::PhongMaterialTextures &textures, std::function<void(Model *)> onLoadComplete);
                void load(const std::string &alias, std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices, std::function<void(Model *)> onLoadComplete);
                Model *get(const std::string &alias) const;
                void remove(const std::string &alias);
                void removeAll();
        };
    }
}

#endif
