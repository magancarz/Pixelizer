#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

class Object;

struct FrameInfo
{
    VkCommandBuffer graphics_command_buffer;
    VkCommandBuffer compute_command_buffer;
    VkExtent2D window_size;
    float delta_time{0};
    uint32_t frame_index{0};

    VkDescriptorSet rendered_to_texture;

    glm::mat4 camera_view_matrix{1.f};
    glm::mat4 camera_projection_matrix{1.f};

    glm::vec3 sun_position{glm::normalize(glm::vec3{1})};
    float weather{0.05f};

    int kernel_size{10};
    float color_weight{0.45f};
    float normal_weight{0.30f};
    float position_weight{0.25f};

    int lod_bias{8};
};
