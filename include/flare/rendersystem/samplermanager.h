#ifndef FLARE_RENDERSYSTEM_SAMPLERMANAGER_H
#define FLARE_RENDERSYSTEM_SAMPLERMANAGER_H

#include <memory>
#include <unordered_map>

#include <flare/rendersystem/sampler.h>

namespace Flare
{
    namespace RenderSystem
    {
        class SamplerManager
        {
            private:
                std::unordered_map<size_t, std::unique_ptr<RenderSystem::Sampler>> samplers;
                std::hash<std::string> stringHasher;
            public:
                SamplerManager() {}
                ~SamplerManager();
                SamplerManager(SamplerManager &&other);
                SamplerManager &operator=(SamplerManager &&other);
                SamplerManager(const SamplerManager &other) = delete;
                SamplerManager &operator=(const SamplerManager &other) = delete;

                RenderSystem::Sampler *get(const std::string &alias) const;
                void insert(std::unique_ptr<RenderSystem::Sampler> &&sampler, const std::string &alias);
                void remove(const std::string &alias);
                void removeAll();
        };
    }
}

#endif
