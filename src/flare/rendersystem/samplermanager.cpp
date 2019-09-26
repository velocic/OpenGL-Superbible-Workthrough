#include <flare/rendersystem/samplermanager.h>

namespace Flare
{
    namespace RenderSystem
    {
        SamplerManager::~SamplerManager()
        {
            removeAll();
        }

        SamplerManager::SamplerManager(SamplerManager &&other)
        :
            samplers(std::move(other.samplers))
        {
        }

        SamplerManager &SamplerManager::operator=(SamplerManager &&other)
        {
            samplers = std::move(other.samplers);
            
            return *this;
        }

        RenderSystem::Sampler *SamplerManager::get(const std::string &alias) const
        {
            auto result = samplers.find(stringHasher(alias));

            if (result != samplers.end()) {
                return result->second.get();
            }

            return nullptr;
        }

        void SamplerManager::insert(std::unique_ptr<RenderSystem::Sampler> &&sampler, const std::string &alias)
        {
            samplers.insert_or_assign(stringHasher(alias), std::move(sampler));
        }

        void SamplerManager::remove(const std::string &alias)
        {
            samplers.erase(stringHasher(alias));
        }

        void SamplerManager::removeAll()
        {
            for (const auto &it : samplers) {
                it.second->destroy();
            }

            samplers.clear();
        }
    }
}
