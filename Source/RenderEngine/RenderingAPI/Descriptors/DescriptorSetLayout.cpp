#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(
        const Device& logical_device, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings)
        : logical_device{logical_device}, bindings{bindings}
{
    std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
    for (auto& kv : bindings)
    {
        set_layout_bindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = set_layout_bindings.data();

    if (vkCreateDescriptorSetLayout(logical_device.handle(), &descriptor_set_layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(logical_device.handle(), descriptor_set_layout, nullptr);
}