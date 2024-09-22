#include "Pipeline.h"

#include <fstream>

#include "VulkanDefines.h"
#include "Assets/Model/Vertex.h"
#include "VulkanUtils.h"

Pipeline::Pipeline(
        const Device& device,
        VkPipelineLayout pipeline_layout,
        const std::string& vertex_file_path,
        const std::string& fragment_file_path,
        const PipelineConfigInfo& config_info)
    : device{device}, pipeline_layout{pipeline_layout}
{
    createGraphicsPipeline(vertex_file_path, fragment_file_path, config_info);
}

Pipeline::~Pipeline()
{
    vkDestroyShaderModule(device.handle(), vertex_shader_module, VulkanDefines::NO_CALLBACK);
    vkDestroyShaderModule(device.handle(), fragment_shader_module, VulkanDefines::NO_CALLBACK);
    vkDestroyPipeline(device.handle(), graphics_pipeline, VulkanDefines::NO_CALLBACK);
}

void Pipeline::bind(VkCommandBuffer command_buffer)
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}

void Pipeline::bindDescriptorSets(VkCommandBuffer command_buffer, VkDescriptorSet* descriptor_sets, uint32_t descriptor_set_count)
{
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_layout,
        0,
        descriptor_set_count,
        descriptor_sets,
        0,
        nullptr);
}

void Pipeline::createGraphicsPipeline(const std::string& vertex_file_path, const std::string& fragment_file_path, const PipelineConfigInfo& config_info)
{
    assert(config_info.pipeline_layout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipeline layout provided in config info!");

    auto vert_code = readFile(vertex_file_path);
    auto frag_code = readFile(fragment_file_path);

    createShaderModule(vert_code, &vertex_shader_module);
    createShaderModule(frag_code, &fragment_shader_module);

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stage_create_infos{};
    shader_stage_create_infos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_infos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stage_create_infos[0].module = vertex_shader_module;
    shader_stage_create_infos[0].pName = "main";
    shader_stage_create_infos[0].flags = 0;
    shader_stage_create_infos[0].pNext = nullptr;
    shader_stage_create_infos[0].pSpecializationInfo = nullptr;

    shader_stage_create_infos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_infos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stage_create_infos[1].module = fragment_shader_module;
    shader_stage_create_infos[1].pName = "main";
    shader_stage_create_infos[1].flags = 0;
    shader_stage_create_infos[1].pNext = nullptr;
    shader_stage_create_infos[1].pSpecializationInfo = nullptr;

    auto& attribute_descriptions = config_info.attribute_descriptions;
    auto& binding_descriptions = config_info.binding_descriptions;

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
    vertex_input_state_create_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions.data();
    vertex_input_state_create_info.pVertexBindingDescriptions = binding_descriptions.data();

    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shader_stage_create_infos.data();
    pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    pipeline_create_info.pInputAssemblyState = &config_info.input_assembly_info;
    pipeline_create_info.pViewportState = &config_info.viewport_info;
    pipeline_create_info.pRasterizationState = &config_info.rasterization_info;
    pipeline_create_info.pMultisampleState = &config_info.multisample_info;
    pipeline_create_info.pColorBlendState = &config_info.color_blend_info;
    pipeline_create_info.pDepthStencilState = &config_info.depth_stencil_info;
    pipeline_create_info.pDynamicState = &config_info.dynamic_state_info;
    pipeline_create_info.pNext = &config_info.rendering_create_info;

    pipeline_create_info.layout = config_info.pipeline_layout;

    pipeline_create_info.basePipelineIndex = -1;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.handle(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &graphics_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }
}

std::vector<char> Pipeline::readFile(const std::string& file_path)
{
    std::ifstream file{file_path, std::ios::ate | std::ios::binary};
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file " + file_path);
    }

    auto file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

void Pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shader_module)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = std::bit_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device.handle(), &create_info, VulkanDefines::NO_CALLBACK, shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module!");
    }
}

void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& config_info)
{
    config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config_info.viewport_info.viewportCount = 1;
    config_info.viewport_info.pViewports = nullptr;
    config_info.viewport_info.scissorCount = 1;
    config_info.viewport_info.pScissors = nullptr;

    config_info.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config_info.rasterization_info.depthClampEnable = VK_FALSE;
    config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    config_info.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    config_info.rasterization_info.lineWidth = 1.0f;
    config_info.rasterization_info.cullMode = VK_CULL_MODE_NONE;
    config_info.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config_info.rasterization_info.depthBiasEnable = VK_FALSE;
    config_info.rasterization_info.depthBiasConstantFactor = 0.0f;
    config_info.rasterization_info.depthBiasClamp = 0.0f;
    config_info.rasterization_info.depthBiasSlopeFactor = 0.0f;

    config_info.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config_info.multisample_info.sampleShadingEnable = VK_FALSE;
    config_info.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config_info.multisample_info.minSampleShading = 1.0f;
    config_info.multisample_info.pSampleMask = nullptr;
    config_info.multisample_info.alphaToCoverageEnable = VK_FALSE;
    config_info.multisample_info.alphaToOneEnable = VK_FALSE;

    config_info.color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config_info.color_blend_attachment.blendEnable = VK_FALSE;
    config_info.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    config_info.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    config_info.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    config_info.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config_info.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config_info.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    config_info.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config_info.color_blend_info.logicOpEnable = VK_FALSE;
    config_info.color_blend_info.logicOp = VK_LOGIC_OP_COPY;
    config_info.color_blend_info.attachmentCount = 1;
    config_info.color_blend_info.pAttachments = &config_info.color_blend_attachment;
    config_info.color_blend_info.blendConstants[0] = 0.0f;
    config_info.color_blend_info.blendConstants[1] = 0.0f;
    config_info.color_blend_info.blendConstants[2] = 0.0f;
    config_info.color_blend_info.blendConstants[3] = 0.0f;

    config_info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config_info.depth_stencil_info.depthTestEnable = VK_TRUE;
    config_info.depth_stencil_info.depthWriteEnable = VK_TRUE;
    config_info.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    config_info.depth_stencil_info.minDepthBounds = 0.0f;
    config_info.depth_stencil_info.maxDepthBounds = 1.0f;
    config_info.depth_stencil_info.stencilTestEnable = VK_FALSE;
    config_info.depth_stencil_info.front = {};
    config_info.depth_stencil_info.back = {};

    config_info.dynamic_state_enables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    config_info.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config_info.dynamic_state_info.pDynamicStates = config_info.dynamic_state_enables.data();
    config_info.dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(config_info.dynamic_state_enables.size());
    config_info.depth_stencil_info.flags = 0;

    config_info.binding_descriptions = VulkanUtils::getVertexBindingDescriptions();
    config_info.attribute_descriptions = VulkanUtils::getVertexAttributeDescriptions();
}
