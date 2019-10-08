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

        void Model::render(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer, size_t instanceCount)
        {
            for (auto &mesh : meshes) {
                mesh->bind(shaderData, mvpMatrixBuffer);
                mesh->render(instanceCount);
            }
        }
    }
}
