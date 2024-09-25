#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

class Mesh;
class Object;

struct FrameInfo
{
    VkCommandBuffer command_buffer{VK_NULL_HANDLE};
    VkExtent2D window_size{};
    float delta_time{0};
    uint32_t frame_index{0};

    glm::mat4 camera_view_matrix{1.f};
    glm::mat4 camera_projection_matrix{1.f};

    Mesh* rendered_model{nullptr};
    glm::mat4 model_matrix{1.f};
    glm::mat4 normal_matrix{1.f};

    uint32_t pixelize{0};
};
