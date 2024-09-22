#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

class Object;

struct FrameInfo
{
    VkCommandBuffer graphics_command_buffer{VK_NULL_HANDLE};
    VkExtent2D window_size{};
    float delta_time{0};
    uint32_t frame_index{0};

    glm::mat4 camera_view_matrix{1.f};
    glm::mat4 camera_projection_matrix{1.f};
};
