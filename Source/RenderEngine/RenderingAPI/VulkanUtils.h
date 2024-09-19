#pragma once

#include <vulkan/vulkan.hpp>

class PhysicalDevice;

class VulkanUtils
{
public:
    static std::vector<VkVertexInputBindingDescription> getVertexBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();
};
