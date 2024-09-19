#pragma once

#include "DescriptorPool.h"
#include "RenderEngine/RenderingAPI/VulkanSystem.h"

class DescriptorPoolBuilder
{
public:
    explicit DescriptorPoolBuilder(VulkanSystem& device) : device{device} {}

    DescriptorPoolBuilder& addPoolSize(VkDescriptorType descriptor_type, uint32_t count);
    DescriptorPoolBuilder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
    DescriptorPoolBuilder& setMaxSets(uint32_t count);
    [[nodiscard]] std::unique_ptr<DescriptorPool> build() const;

private:
    VulkanSystem& device;
    std::vector<VkDescriptorPoolSize> pool_sizes{};
    uint32_t max_sets{1000};
    VkDescriptorPoolCreateFlags pool_flags{0};
};
