#pragma once

#include <filesystem>

class VulkanDefines
{
public:
    inline static const VkAllocationCallbacks* NO_CALLBACK = nullptr;
    inline static const std::filesystem::path SHADER_DIRECTORY_PATH{"Shaders"};
};

#define VK_CHECK(x)                                                    \
    do                                                                 \
    {                                                                  \
        VkResult err = x;                                              \
        if (err)                                                       \
        {                                                              \
            std::cout <<"Detected Vulkan error: " << err << std::endl; \
            abort();                                                   \
        }                                                              \
    } while (0)