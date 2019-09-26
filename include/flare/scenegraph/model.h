#ifndef FLARE_SCENEGRAPH_MODEL_H
#define FLARE_SCENEGRAPH_MODEL_H

#include <memory>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <flare/rendersystem/texture.h>
#include <flare/rendersystem/shadermanager.h>
#include <flare/scenegraph/mesh.h>

namespace Flare
{
    namespace SceneGraph
    {
        class Model
        {
            private:
                std::vector<std::unique_ptr<Mesh>> meshes;
            public:
                Model(std::vector<std::unique_ptr<Mesh>> &&meshes);
                ~Model();
                Model(Model &&other);
                Model &operator=(Model &&other);
                Model(const Model &other) = delete;
                Model &operator=(const Model &other) = delete;

                void render(RenderSystem::ShaderData shaderData, size_t instanceCount);
        };
    }
}

#endif
