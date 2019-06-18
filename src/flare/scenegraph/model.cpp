#include <flare/scenegraph/model.h>

#include <stdexcept>

namespace Flare
{
    namespace SceneGraph
    {
        Model::Model(std::vector<std::unique_ptr<Mesh>> &&meshes)
        :
            meshes(std::move(meshes))
        {
        }

        Model::~Model()
        {
        }

        Model::Model(Model &&other)
        :
            meshes(std::move(other.meshes))
        {
        }

        Model &Model::operator=(Model &&other)
        {
            meshes = std::move(other.meshes);

            return *this;
        }

        void Model::render(RenderSystem::ShaderProgram *shader, size_t instanceCount)
        {
            for (auto &mesh : meshes) {
                mesh->bind(shader);
                mesh->render(instanceCount);
            }
        }
    }
}
