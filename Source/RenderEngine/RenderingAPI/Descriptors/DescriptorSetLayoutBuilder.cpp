#include "DescriptorSetLayoutBuilder.h"

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptor_type,
        VkShaderStageFlags stage_flags,
        uint32_t count)
{
    assert(!bindings.contains(binding) && "Binding already in use");
    assert(count >= 1 && "Must be at least one binding");

    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = count;
    layout_binding.stageFlags = stage_flags;
    bindings[binding] = layout_binding;

    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayoutBuilder::build() const
{
    if (bindings.empty())
    {
        return nullptr;
    }

    return std::make_unique<DescriptorSetLayout>(logical_device, bindings);
}