#include "DescriptorPoolBuilder.h"

DescriptorPoolBuilder& DescriptorPoolBuilder::addPoolSize(
        VkDescriptorType descriptor_type, uint32_t count)
{
    pool_sizes.push_back({descriptor_type, count});
    return *this;
}

DescriptorPoolBuilder& DescriptorPoolBuilder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags)
{
    pool_flags = flags;
    return *this;
}

DescriptorPoolBuilder& DescriptorPoolBuilder::setMaxSets(uint32_t count)
{
    max_sets = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPoolBuilder::build() const
{
    if (pool_sizes.empty())
    {
        return nullptr;
    }

    return std::make_unique<DescriptorPool>(device.getLogicalDevice(), max_sets, pool_flags, pool_sizes);
}