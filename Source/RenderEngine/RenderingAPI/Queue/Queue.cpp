#include "Queue.h"

#include "RenderEngine/RenderingAPI/Device.h"

Queue::Queue(Device& device, uint32_t queue_family_index)
    : device{device}, queue_family_index{queue_family_index},
    queue{createQueue(queue_family_index)}, queue_mutex{device.fetchQueueMutex(queue_family_index, 0)} {}

VkQueue Queue::createQueue(uint32_t queue_family_index)
{
    VkQueue queue{VK_NULL_HANDLE};
    vkGetDeviceQueue(device.handle(), queue_family_index, 0, &queue);
    return queue;
}

VkResult Queue::submitCommandBuffer(const std::vector<VkSubmitInfo>& submit_infos, VkFence fence) const
{
    return submitCommandBuffer(submit_infos.size(), submit_infos.data(), fence);
}

VkResult Queue::submitCommandBuffer(uint32_t submit_info_count, const VkSubmitInfo* submit_infos, VkFence fence) const
{
    std::lock_guard lock{queue_mutex};
    return vkQueueSubmit(queue, submit_info_count, submit_infos, fence);
}

VkResult Queue::present(const VkPresentInfoKHR& present_info) const
{
    std::lock_guard lock{queue_mutex};
    return vkQueuePresentKHR(queue, &present_info);
}
