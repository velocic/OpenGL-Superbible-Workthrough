#ifndef FLARE_SCENEGRAPH_MODEL_H
#define FLARE_SCENEGRAPH_MODEL_H

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Model
        {
            private:
                std::vector<Mesh *> meshes;

                void loadModel(const std::string &filePath);
                void processNode(aiNode *node, const aiScene *scene);
                Mesh *processMesh(aiMesh *mesh, const aiScene *scene);
                std::vector<std::string, RenderSystem::Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
            public:
                Model(const std::string &filePath);
                ~Model();
                Model(Model &&other);
                Model &operator=(Model &&other);
                Model(const Model &other) = delete;
                Model &operator=(const Model &other) = delete;
        };
    }
}

#endif
