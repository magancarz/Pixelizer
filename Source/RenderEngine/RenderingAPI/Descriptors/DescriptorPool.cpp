#include "DescriptorPool.h"

#include <iostream>

#include "RenderEngine/RenderingAPI/VulkanDefines.h"

DescriptorPool::DescriptorPool(
        const Device& device,
        uint32_t max_sets,
        VkDescriptorPoolCreateFlags pool_flags,
        const std::vector<VkDescriptorPoolSize>& pool_sizes)
        : device{device}, descriptor_pool{createDescriptorPool(max_sets, pool_flags, pool_sizes)} {}

VkDescriptorPool DescriptorPool::createDescriptorPool(
        uint32_t max_sets,
        VkDescriptorPoolCreateFlags pool_flags,
        const std::vector<VkDescriptorPoolSize>& pool_sizes) const
{
    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    descriptor_pool_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_info.maxSets = max_sets;
    descriptor_pool_info.flags = pool_flags;

    VkDescriptorPool new_descriptor_pool{VK_NULL_HANDLE};
    VK_CHECK(vkCreateDescriptorPool(device.handle(), &descriptor_pool_info, VulkanDefines::NO_CALLBACK, &new_descriptor_pool));

    return new_descriptor_pool;
}

bool DescriptorPool::allocateDescriptor(VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.pSetLayouts = &descriptor_set_layout;
    alloc_info.descriptorSetCount = 1;

    return vkAllocateDescriptorSets(device.handle(), &alloc_info, &descriptor) == VK_SUCCESS;
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(device.handle(), descriptor_pool, nullptr);
}

void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
{
    vkFreeDescriptorSets(
        device.handle(),
        descriptor_pool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
}

void DescriptorPool::resetPool() const
{
    vkResetDescriptorPool(device.handle(), descriptor_pool, 0);
}
