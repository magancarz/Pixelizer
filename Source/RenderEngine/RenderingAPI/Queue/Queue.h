#pragma once

#include <mutex>
#include <vulkan/vulkan.hpp>

#include "../CommandBuffer/CommandPool.h"

class Device;

class Queue
{
public:
    Queue(Device& device, uint32_t queue_family_index);

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
    Queue(const Queue&&) = delete;
    Queue& operator=(const Queue&&) = delete;

    [[nodiscard]] VkQueue getQueue() const { return queue; }
    [[nodiscard]] uint32_t getQueueFamilyIndex() const { return queue_family_index; }

    VkResult submitCommandBuffer(const std::vector<VkSubmitInfo>& submit_infos, VkFence fence = VK_NULL_HANDLE) const;
    VkResult submitCommandBuffer(uint32_t submit_info_count, const VkSubmitInfo* submit_infos, VkFence fence = VK_NULL_HANDLE) const;
    [[nodiscard]] VkResult present(const VkPresentInfoKHR& present_info) const;

private:
    Device& device;
    uint32_t queue_family_index{0};
    VkQueue queue;
    std::mutex& queue_mutex;

    VkQueue createQueue(uint32_t queue_family_index);
};
