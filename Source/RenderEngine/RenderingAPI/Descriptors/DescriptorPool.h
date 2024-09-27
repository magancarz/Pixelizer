#pragma once

#include <memory>

#include "RenderEngine/RenderingAPI/VulkanSystem.h"

class DescriptorPool
{
public:
    DescriptorPool(
        const Device& device,
        uint32_t max_sets,
        VkDescriptorPoolCreateFlags pool_flags,
        const std::vector<VkDescriptorPoolSize>& pool_sizes);
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    [[nodiscard]] VkDescriptorPool descriptorPool() const { return descriptor_pool; }

    bool allocateDescriptor(VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const;
    void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
    void resetPool() const;

private:
    const Device& device;

    [[nodiscard]] VkDescriptorPool createDescriptorPool(
        uint32_t max_sets,
        VkDescriptorPoolCreateFlags pool_flags,
        const std::vector<VkDescriptorPoolSize>& pool_sizes) const;

    VkDescriptorPool descriptor_pool;

    friend class DescriptorWriter;
};