#include "CommandPool.h"

#include "RenderEngine/RenderingAPI/Queue/Queue.h"
#include "RenderEngine/RenderingAPI/VulkanDefines.h"
#include "RenderEngine/RenderingAPI/CommandBuffer/SingleTimeCommandBuffer.h"
#include "RenderEngine/RenderingAPI/Device.h"

CommandPool::CommandPool(const Device& device, const Queue& queue)
    : device{device}, queue{queue}, command_pool{createCommandPool(queue.getQueueFamilyIndex())} {}

VkCommandPool CommandPool::createCommandPool(uint32_t queue_family)
{
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family;
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool command_pool;
    if (vkCreateCommandPool(device.handle(), &pool_info, VulkanDefines::NO_CALLBACK, &command_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool!");
    }

    return command_pool;
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device.handle(), command_pool, VulkanDefines::NO_CALLBACK);
}

CommandBuffer CommandPool::createCommandBuffer() const
{
    VkCommandBuffer command_buffer{VK_NULL_HANDLE};
    allocateCommandBuffers(&command_buffer);
    return CommandBuffer{*this, command_buffer};
}

void CommandPool::allocateCommandBuffers(VkCommandBuffer* command_buffers, uint32_t count) const
{
    VkCommandBufferAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandPool = command_pool;
    allocate_info.commandBufferCount = count;

    std::lock_guard lock{command_pool_mutex};
    vkAllocateCommandBuffers(device.handle(), &allocate_info, command_buffers);
}

std::vector<CommandBuffer> CommandPool::createCommandBuffers(uint32_t count) const
{
    std::vector<VkCommandBuffer> command_buffers(count);
    allocateCommandBuffers(command_buffers.data(), count);

    std::vector<CommandBuffer> result_command_buffers;
    result_command_buffers.reserve(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        result_command_buffers.emplace_back(*this, command_buffers[i]);
    }

    return result_command_buffers;
}

SingleTimeCommandBuffer CommandPool::createSingleTimeCommandBuffer() const
{
    VkCommandBuffer command_buffer{VK_NULL_HANDLE};
    allocateCommandBuffers(&command_buffer);
    return SingleTimeCommandBuffer{device, *this, queue, command_buffer};
}

void CommandPool::freeCommandBuffers(uint32_t count, CommandBuffer* command_buffers) const
{
    std::vector<VkCommandBuffer> command_buffer_handles(count);
    for (std::size_t i = 0; i < count; ++i)
    {
        command_buffer_handles[i] = command_buffers[i].handle();
        command_buffers[i].invalidate();
    }

    freeCommandBufferHandles(count, command_buffer_handles.data());
}

void CommandPool::freeCommandBufferHandles(uint32_t count, VkCommandBuffer* command_buffer_handles) const
{
    std::lock_guard lock{command_pool_mutex};
    vkFreeCommandBuffers(device.handle(), command_pool, count, command_buffer_handles);
}

void CommandPool::beginCommandBuffer(VkCommandBuffer command_buffer, const VkCommandBufferBeginInfo& begin_info) const
{
    command_buffer_recording_lock.lock();
    vkBeginCommandBuffer(command_buffer, &begin_info);
}

void CommandPool::endCommandBuffer(VkCommandBuffer command_buffer) const
{
    vkEndCommandBuffer(command_buffer);
    command_buffer_recording_lock.unlock();
}

void CommandPool::resetCommandBuffer(VkCommandBuffer command_buffer, VkCommandBufferResetFlags flags) const
{
    std::lock_guard lock{command_pool_mutex};
    vkResetCommandBuffer(command_buffer, flags);
}
