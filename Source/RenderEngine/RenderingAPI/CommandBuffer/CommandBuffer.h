#pragma once

#include <vulkan/vulkan.hpp>

class CommandPool;

class CommandBuffer
{
public:
    CommandBuffer(const CommandPool& owning_command_pool, VkCommandBuffer command_buffer);
    virtual ~CommandBuffer();

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer& operator=(CommandBuffer&& other) noexcept;

    virtual VkCommandBuffer beginRecording();
    virtual void endRecording();
    void reset();

    [[nodiscard]] const VkCommandBuffer& handle() const { return command_buffer; }

private:
    const CommandPool& owning_command_pool;
    VkCommandBuffer command_buffer;
    bool currently_recording{false};

    void invalidate();

    friend class CommandPool;
};
