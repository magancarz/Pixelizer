#pragma once

#include <string>
#include <vector>

#include "PipelineConfigInfo.h"
#include "VulkanSystem.h"

class Pipeline
{
public:
    Pipeline(
        const Device& device,
        const std::string& vertex_file_path,
        const std::string& fragment_file_path,
        const PipelineConfigInfo& config_info);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void bind(VkCommandBuffer command_buffer);
    void bindDescriptorSets(
        VkCommandBuffer command_buffer,
        const VkDescriptorSet* descriptor_sets,
        uint32_t first_set,
        uint32_t descriptor_set_count);

    template <typename T>
    void pushConstants(VkCommandBuffer command_buffer, VkShaderStageFlagBits shader_stage_flag_bits, uint32_t offset, T* push_constant)
    {
        vkCmdPushConstants(
            command_buffer,
            pipeline_layout,
            shader_stage_flag_bits,
            offset,
            sizeof(T),
            push_constant);
    }

    static PipelineConfigInfo defaultPipelineConfigInfo();

private:
    static std::vector<char> readFile(const std::string& file_path);
    void createPipelineLayout(const PipelineConfigInfo& config_info);
    void createGraphicsPipeline(const std::string& vertex_file_path, const std::string& fragment_file_path, const PipelineConfigInfo& config_info);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shader_module);

    const Device& device;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
    VkShaderModule vertex_shader_module;
    VkShaderModule fragment_shader_module;
};
