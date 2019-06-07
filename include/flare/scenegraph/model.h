#ifndef FLARE_SCENEGRAPH_MODEL_H
#define FLARE_SCENEGRAPH_MODEL_H

#include <memory>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <flare/rendersystem/texture.h>
#include <flare/scenegraph/mesh.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Model
        {
            private:
                std::string directory;
                std::vector<std::unique_ptr<Mesh>> meshes;

                void loadModel(const std::string &filePath);
                void processNode(aiNode *node, const aiScene *scene);
                Mesh *processMesh(aiMesh *mesh, const aiScene *scene);
                std::vector<std::pair<std::string, RenderSystem::Texture *>> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
            public:
                Model(const std::string &filePath);
                ~Model();
                Model(Model &&other);
                Model &operator=(Model &&other);
                Model(const Model &other) = delete;
                Model &operator=(const Model &other) = delete;

                void render(RenderSystem::ShaderProgram *shader, size_t instanceCount);
        };
    }
}

#endif
