#include "DescriptorWriter.h"

#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"

DescriptorWriter::DescriptorWriter(DescriptorSetLayout& set_layout, DescriptorPool& pool)
        : set_layout{set_layout}, pool{pool} {}

DescriptorWriter& DescriptorWriter::writeBuffer(
        uint32_t binding, const VkDescriptorBufferInfo* buffer_info)
{
    assert(set_layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& binding_description = set_layout.bindings[binding];

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = binding_description.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = buffer_info;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(
        uint32_t binding, const VkDescriptorImageInfo* image_info, uint32_t count)
{
    assert(set_layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& binding_description = set_layout.bindings[binding];

    assert(binding_description.descriptorCount == count);

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = binding_description.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = image_info;
    write.descriptorCount = count;

    writes.push_back(write);
    return *this;
}

bool DescriptorWriter::build(VkDescriptorSet& set)
{
    if (!pool.allocateDescriptor(set_layout.getDescriptorSetLayout(), set))
    {
        return false;
    }

    overwrite(set);
    return true;
}

void DescriptorWriter::overwrite(VkDescriptorSet& set)
{
    for (auto& write : writes)
    {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.device.handle(), writes.size(), writes.data(), 0, nullptr);
}