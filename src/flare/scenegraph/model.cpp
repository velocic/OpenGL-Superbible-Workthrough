#include <flare/scenegraph/model.h>

#include <stdexcept>

namespace Flare
{
    namespace SceneGraph
    {
        Model::Model(const std::string &filePath)
        {
        }

        Model::~Model()
        {
        }

        Model::Model(Model &&other)
        {
        }

        Model &Model::operator=(Model &&other)
        {
            return *this;
        }

        void Model::loadModel(const std::string &filePath)
        {
            auto importer = Assimp::Importer{}; 
            const auto scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

            if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
                throw std::runtime_error(importer.GetErrorString());
            }

            directory = filePath.substr(0, filePath.find_last_of('/'));

            processNode(scene->mRootNode, scene);
        }

        void Model::processNode(aiNode *node, const aiScene *scene)
        {
            //Process all meshes attached to this node
            for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
                auto mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(
                    std::unique_ptr<Mesh>(
                        processMesh(mesh, scene)
                    )
                );
            }

            //Process all nodes of this node's children
            for (unsigned int i = 0; i < node->mNumChildren; ++i) {
                processNode(node->mChildren[i], scene);
            }
        }

        Mesh *Model::processMesh(aiMesh *mesh, const aiScene *scene)
        {
        }

        std::vector<std::pair<std::string, RenderSystem::Texture *>> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
        {
            return std::vector<std::pair<std::string, RenderSystem::Texture *>>{};
        }

        void Model::render(RenderSystem::ShaderProgram *shader, size_t instanceCount)
        {
            //For each mesh
            //  bind
            //  draw once for each world matrix in buffer from SceneNode (which may
            //      not be visible here if using glDrawElementsInstanced or something.
            //      that binding will happen in the scene node before this call, and we
            //      may just have to assume it is bound correctly. Or it can be passed
            //      in here. Maybe just pass in an instance count?)
        }
    }
}
