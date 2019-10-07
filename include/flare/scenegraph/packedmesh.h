#ifndef FLARE_SCENEGRAPH_PACKEDMESH_H
#define FLARE_SCENEGRAPH_PACKEDMESH_H

namespace Flare
{
    namespace SceneGraph
    {
        class PackedMesh
        {
            private:
                std::vector<std::variant<RenderSystem::PhongMaterialTextures, RenderSystem::PBRMaterialTextures, nullptr_t>> textures;
                std::unique_ptr<RenderSystem::Buffer> VBO;
                std::unique_ptr<RenderSystem::Buffer> EBO;
                std::vector<size_t> baseVertices;
                std::vector<size_t> baseInstances;

                void populateBuffers(std::vector<DataTypes::Vertex> &&vertices, std::vector<unsigned int> &&indices);
            public:
        };
    }
}

#endif
