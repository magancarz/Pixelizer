#pragma once

#include "RenderEngine/RenderingAPI/VulkanSystem.h"

class DescriptorPool;
class DescriptorSetLayout;

class DescriptorWriter
{
public:
    DescriptorWriter(DescriptorSetLayout& set_layout, DescriptorPool& pool);

    DescriptorWriter& writeBuffer(uint32_t binding, const VkDescriptorBufferInfo* buffer_info);
    DescriptorWriter& writeImage(uint32_t binding, const VkDescriptorImageInfo* image_info, uint32_t count = 1);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);

private:
    DescriptorSetLayout& set_layout;
    DescriptorPool& pool;
    std::vector<VkWriteDescriptorSet> writes;
};
