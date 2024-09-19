#pragma once

#include <vulkan/vulkan_core.h>

#include "RenderEngine/RenderingAPI/Device.h"

class Fence
{
public:
    explicit Fence(const Device& logical_device, VkFenceCreateFlags flags = 0);
    ~Fence();

    void reset();
    void wait() const;

    [[nodiscard]] const VkFence& handle() const { return fence; }

private:
    const Device& logical_device;

    VkFence createFence(VkFenceCreateFlags flags);

    VkFence fence{VK_NULL_HANDLE};
};
