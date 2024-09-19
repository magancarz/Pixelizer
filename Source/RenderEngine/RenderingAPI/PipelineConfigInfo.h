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
    VkPipelineDynamicStateCreateInfo dynamic_state_info;
    VkPipelineLayout pipeline_layout{nullptr};
    VkRenderPass render_pass{nullptr};
    uint32_t subpass{0};
};