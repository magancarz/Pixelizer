#include "Fence.h"

#include "Logs/LogSystem.h"
#include "RenderEngine/RenderingAPI/VulkanDefines.h"

Fence::Fence(const Device& logical_device, VkFenceCreateFlags flags)
    : logical_device{logical_device}, fence{createFence(flags)} {}

VkFence Fence::createFence(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = flags;

    VkFence fence{VK_NULL_HANDLE};

    if (vkCreateFence(logical_device.handle(), &fence_info, VulkanDefines::NO_CALLBACK, &fence) != VK_SUCCESS)
    {
        const char* message = "Failed to create compute synchronization objects for a frame!";
        LogSystem::log(LogSeverity::FATAL, message);
        throw std::runtime_error(message);
    }

    return fence;
}

Fence::~Fence()
{
    vkDestroyFence(logical_device.handle(), fence, VulkanDefines::NO_CALLBACK);
}

void Fence::reset()
{
    vkResetFences(logical_device.handle(), 1, &fence);
}

void Fence::wait() const
{
    vkWaitForFences(logical_device.handle(), 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
}
