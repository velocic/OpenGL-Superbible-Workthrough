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
