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

        std::unique_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene)
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

        std::vector<std::pair<std::string, RenderSystem::Texture *>> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
        {
            auto loadedTextures = std::vector<std::pair<std::string, RenderSystem::Texture *>>{};

            for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
                aiString textureName;
                mat->GetTexture(type, i, &textureName);

                //TODO: rest of assimp texture loading
            }
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
