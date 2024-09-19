#pragma once

#include <string>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include "VulkanSystem.h"

class ShaderModule
{
public:
    ShaderModule(const Device& logical_device, const std::string& shader_code_file, VkShaderStageFlagBits shader_stage);
    ~ShaderModule();

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;

    [[nodiscard]] VkShaderModule getShaderModule() const { return shader_module; }

    inline static const std::unordered_map<VkShaderStageFlagBits, const char* const> SHADER_CODE_EXTENSIONS
    {
        {VK_SHADER_STAGE_COMPUTE_BIT, ".comp.spv"},
        {VK_SHADER_STAGE_VERTEX_BIT, ".vert.spv"},
        {VK_SHADER_STAGE_FRAGMENT_BIT, ".frag.spv"}
    };

private:
    const Device& logical_device;

    static std::string getPathToShaderCodeFile(const std::string& shader_code_file, VkShaderStageFlagBits shader_stage);
    void createShaderModule(const std::string& path_to_shader_code);
    static std::vector<uint32_t> loadShaderSourceCode(const std::string& path_to_shader_code);

    VkShaderModule shader_module{VK_NULL_HANDLE};
};
