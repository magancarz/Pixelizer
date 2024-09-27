#include "ComputePipeline.h"

#include "RenderEngine/RenderingAPI/VulkanDefines.h"

ComputePipeline::ComputePipeline(
        const Device& logical_device,
        const std::vector<VkPushConstantRange>& push_constant_ranges,
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
        std::unique_ptr<ShaderModule> compute_shader_module)
    : logical_device{logical_device}
{
    createComputePipeline(push_constant_ranges, descriptor_set_layouts, std::move(compute_shader_module));
}

void ComputePipeline::createComputePipeline(
        const std::vector<VkPushConstantRange>& push_constant_ranges,
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
        std::unique_ptr<ShaderModule> compute_shader_module)
{
    VkPipelineLayoutCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.setLayoutCount = descriptor_set_layouts.size();
    create_info.pSetLayouts = descriptor_set_layouts.data();
    create_info.pushConstantRangeCount = push_constant_ranges.size();
    create_info.pPushConstantRanges = push_constant_ranges.data();

    if (vkCreatePipelineLayout(logical_device.handle(), &create_info, VulkanDefines::NO_CALLBACK, &pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Unable to create compute pipeline layout!");
    }

    VkComputePipelineCreateInfo compute_pipeline_create_info{};
    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.layout = pipeline_layout;

    VkPipelineShaderStageCreateInfo shader_stage_create_info{};
    shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_info.module = compute_shader_module->getShaderModule();
    shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shader_stage_create_info.pName = "main";

    compute_pipeline_create_info.stage = shader_stage_create_info;

    if (vkCreateComputePipelines(logical_device.handle(), {}, 1, &compute_pipeline_create_info, VulkanDefines::NO_CALLBACK, &pipeline_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("Unable to create compute pipeline!");
    }
}

ComputePipeline::~ComputePipeline() noexcept
{
    vkDestroyPipeline(logical_device.handle(), pipeline_handle, VulkanDefines::NO_CALLBACK);
    vkDestroyPipelineLayout(logical_device.handle(), pipeline_layout, VulkanDefines::NO_CALLBACK);
}

void ComputePipeline::bind(VkCommandBuffer command_buffer)
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_handle);
}

void ComputePipeline::bindDescriptorSets(VkCommandBuffer command_buffer, const std::vector<VkDescriptorSet>& descriptor_sets)
{
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        pipeline_layout,
        0,
        static_cast<uint32_t>(descriptor_sets.size()),
        descriptor_sets.data(),
        0,
        nullptr);
}

void ComputePipeline::dispatch(VkCommandBuffer command_buffer, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
    vkCmdDispatch(command_buffer, group_count_x, group_count_y, group_count_z);
}
