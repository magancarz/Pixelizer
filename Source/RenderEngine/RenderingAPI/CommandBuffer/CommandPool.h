#pragma once

#include <future>
#include <mutex>

#include <vulkan/vulkan.hpp>

#include "RenderEngine/RenderingAPI/CommandBuffer/CommandBuffer.h"

class Device;
class SingleTimeCommandBuffer;
class Queue;

class CommandPool
{
public:
    CommandPool(const Device& device, const Queue& queue);
    ~CommandPool();

    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool(const CommandPool&&) = delete;
    CommandPool& operator=(const CommandPool&&) = delete;

    [[nodiscard]] CommandBuffer createCommandBuffer() const;
    [[nodiscard]] std::vector<CommandBuffer> createCommandBuffers(uint32_t count) const;
    [[nodiscard]] SingleTimeCommandBuffer createSingleTimeCommandBuffer() const;

    void freeCommandBuffers(uint32_t count, CommandBuffer* command_buffers) const;

    [[nodiscard]] VkCommandPool handle() const { return command_pool; }

private:
    const Device& device;
    const Queue& queue;
    VkCommandPool command_pool;
    mutable std::mutex command_pool_mutex;

    VkCommandPool createCommandPool(uint32_t queue_family_index);
    void allocateCommandBuffers(VkCommandBuffer* command_buffers, uint32_t count = 1) const;

    void freeCommandBufferHandles(uint32_t count, VkCommandBuffer* command_buffer_handles) const;

    mutable std::unique_lock<std::mutex> command_buffer_recording_lock{command_pool_mutex, std::defer_lock};
    void beginCommandBuffer(VkCommandBuffer command_buffer, const VkCommandBufferBeginInfo& begin_info) const;
    void endCommandBuffer(VkCommandBuffer command_buffer) const;
    void resetCommandBuffer(VkCommandBuffer command_buffer, VkCommandBufferResetFlags flags = 0) const;

    friend class CommandBuffer;
    friend class SingleTimeCommandBuffer;
};
