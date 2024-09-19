#pragma once

#include <filesystem>

class VulkanDefines
{
public:
    inline static const VkAllocationCallbacks* NO_CALLBACK = nullptr;
    inline static const std::filesystem::path SHADER_DIRECTORY_PATH{"Shaders"};
};