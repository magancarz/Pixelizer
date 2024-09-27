#pragma once

#include <unordered_map>

#include "RenderEngine/RenderingAPI/VulkanSystem.h"

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(const Device& logical_device, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout&) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptor_set_layout; }

private:
    const Device& logical_device;
    VkDescriptorSetLayout descriptor_set_layout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class DescriptorWriter;
};