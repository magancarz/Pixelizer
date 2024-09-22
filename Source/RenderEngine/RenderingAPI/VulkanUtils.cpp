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
    attribute_description.emplace_back(0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, position)));
    attribute_description.emplace_back(1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, normal)));
    attribute_description.emplace_back(2, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, uv)));

    return attribute_description;
}
