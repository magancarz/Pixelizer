#pragma once
#include "RenderEngine/RenderingAPI/Device.h"

class Semaphore
{
public:
    explicit Semaphore(VkDevice device, VkSemaphoreCreateFlags flags = 0);
    ~Semaphore();

    [[nodiscard]] const VkSemaphore& handle() const { return semaphore; }

private:
    VkDevice device{VK_NULL_HANDLE};

    VkSemaphore createSemaphore(VkSemaphoreCreateFlags flags);

    VkSemaphore semaphore{VK_NULL_HANDLE};
};
