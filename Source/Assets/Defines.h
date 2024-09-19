#pragma once

#include <filesystem>

#include <vulkan/vulkan.hpp>

namespace Assets
{
    inline constexpr const char* DEFAULT_MESH_NAME{"cube"};
    inline constexpr const char* EMPTY_MESH_NAME{"__invalid_mesh__"};
    inline constexpr const char* DEBUG_MESH_NAME{"__debug_mesh__"};

    inline constexpr const char* DEFAULT_MATERIAL_NAME{"white"};
    inline constexpr const char* DEBUG_MATERIAL_NAME{"__debug__"};

    inline constexpr const char* DEFAULT_DIFFUSE_TEXTURE{"white.png"};
    inline constexpr const char* DEFAULT_NORMAL_MAP{"default_normal_map.png"};
    inline constexpr const char* EMPTY_TEXTURE_NAME{"__invalid_texture__"};
    inline constexpr const char* DEBUG_DIFFUSE_TEXTURE_NAME{"white.png"};
    inline constexpr const char* DEBUG_NORMAL_MAP_NAME{"default_normal_map.png"};

    inline static const std::filesystem::path RESOURCES_DIRECTORY_PATH{"Resources"};
    inline static const std::filesystem::path MODELS_DIRECTORY_PATH{"Resources/Models"};
    inline static const std::filesystem::path MATERIALS_DIRECTORY_PATH{"Resources/Materials"};
    inline static const std::filesystem::path TEXTURES_DIRECTORY_PATH{"Resources/Textures"};

    inline constexpr VkFormat DIFFUSE_TEXTURE_FORMAT{VK_FORMAT_R8G8B8A8_SRGB};
    inline constexpr VkFormat NORMAL_MAP_FORMAT{VK_FORMAT_R8G8B8A8_UNORM};
}