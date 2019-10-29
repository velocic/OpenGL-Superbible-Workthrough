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

        size_t Model::getMeshCount() const
        {
            auto result = size_t{0};

            for (const auto &mesh : meshes) {
                result += mesh->getPackedMeshCount();
            }

            return result;
        }

        void Model::render(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer, size_t instanceCount)
        {
            for (auto &mesh : meshes) {
                mesh->bind(shaderData, mvpMatrixBuffer);
                mesh->render(instanceCount);
            }
        }

        std::vector<Mesh::SortableDrawElementsIndirectCommand> Model::getIndirectDrawCommands(RenderSystem::ShaderData shaderData, const RenderSystem::Buffer &mvpMatrixBuffer, size_t instanceCount) const
        {
            auto result = std::vector<Mesh::SortableDrawElementsIndirectCommand>{};

            for (auto &mesh : meshes) {
                mesh->bind(shaderData, mvpMatrixBuffer);
                auto meshDrawCommands = mesh->getIndirectDrawCommands(instanceCount);
                std::move(meshDrawCommands.begin(), meshDrawCommands.end(), std::back_inserter(result));
            }

            return result;
        }
    }
}
