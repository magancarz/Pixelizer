#include "VulkanUtils.h"

#include "Assets/Model/Vertex.h"
#include "RenderEngine/RenderingAPI/PhysicalDevice.h"

std::vector<VkVertexInputBindingDescription> VulkanUtils::getVertexBindingDescriptions()
{
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return {binding};
}

std::vector<VkVertexInputAttributeDescription> VulkanUtils::getVertexAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attribute_description{};
    attribute_description.push_back({.location = 0, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = static_cast<uint32_t>(offsetof(Vertex, position))});
    attribute_description.push_back({.location = 1, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT, .offset = static_cast<uint32_t>(offsetof(Vertex, uv))});

    return attribute_description;
}
