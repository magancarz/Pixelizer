#pragma once
#include "RenderEngine/RenderingAPI/ShaderModule.h"

class ComputePipeline
{
public:
    explicit ComputePipeline(
        const Device& logical_device,
        const std::vector<VkPushConstantRange>& push_constant_ranges,
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
        std::unique_ptr<ShaderModule> compute_shader_module);
    ~ComputePipeline() noexcept;

    void bind(VkCommandBuffer command_buffer);
    void bindDescriptorSets(VkCommandBuffer command_buffer, const std::vector<VkDescriptorSet>& descriptor_sets);

    template <typename T>
    void pushConstants(VkCommandBuffer command_buffer, T* push_constant)
    {
        vkCmdPushConstants(
            command_buffer,
            pipeline_layout,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0,
            sizeof(T),
            push_constant);
    }

    void dispatch(VkCommandBuffer command_buffer, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

private:
    const Device& logical_device;
    std::unique_ptr<ShaderModule> compute_shader_module;

    void createComputePipeline(
        const std::vector<VkPushConstantRange>& push_constant_ranges,
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
        std::unique_ptr<ShaderModule> compute_shader_module);

    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};
    VkPipeline pipeline_handle{VK_NULL_HANDLE};
};
