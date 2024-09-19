#pragma once

#include "DescriptorSetLayout.h"
#include "RenderEngine/RenderingAPI/VulkanSystem.h"

class DescriptorSetLayoutBuilder
{
public:
    explicit DescriptorSetLayoutBuilder(const Device& logical_device) : logical_device{logical_device} {}

    DescriptorSetLayoutBuilder& addBinding(
        uint32_t binding,
        VkDescriptorType descriptor_type,
        VkShaderStageFlags stage_flags,
        uint32_t count = 1);
    [[nodiscard]] std::unique_ptr<DescriptorSetLayout> build() const;

private:
    const Device& logical_device;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
};
