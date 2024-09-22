#pragma once

#include <vulkan/vulkan.hpp>

struct PipelineConfigInfo
{
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    std::vector<VkVertexInputBindingDescription> binding_descriptions{};
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
    VkPipelineViewportStateCreateInfo viewport_info{};
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    VkPipelineRasterizationStateCreateInfo rasterization_info{};
    VkPipelineMultisampleStateCreateInfo multisample_info{};
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    VkPipelineColorBlendStateCreateInfo color_blend_info{};
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
    std::vector<VkDynamicState> dynamic_state_enables;
    VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    VkPipelineRenderingCreateInfoKHR rendering_create_info{};
    VkPipelineLayout pipeline_layout{nullptr};
};