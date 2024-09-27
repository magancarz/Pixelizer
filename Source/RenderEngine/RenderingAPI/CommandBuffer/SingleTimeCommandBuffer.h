#pragma once

#include "RenderEngine/RenderingAPI/Queue/Queue.h"
#include "RenderEngine/RenderingAPI/Synchronization/Fence.h"

class SingleTimeCommandBuffer
{
public:
    SingleTimeCommandBuffer(const Device& logical_device, const CommandPool& owning_command_pool, const Queue& queue, VkCommandBuffer command_buffer);
    ~SingleTimeCommandBuffer();

    SingleTimeCommandBuffer(const SingleTimeCommandBuffer&) = delete;
    SingleTimeCommandBuffer& operator=(const SingleTimeCommandBuffer&) = delete;
    SingleTimeCommandBuffer(const SingleTimeCommandBuffer&&) = delete;
    SingleTimeCommandBuffer& operator=(const SingleTimeCommandBuffer&&) = delete;

    [[nodiscard]] const VkCommandBuffer& handle() const { return command_buffer; }

    [[nodiscard]] VkCommandBuffer beginRecording();
    [[nodiscard]] const Fence& endRecordingAndSubmit();

private:
    const CommandPool& owning_command_pool;
    const Queue& queue;
    VkCommandBuffer command_buffer;
    Fence fence;
    bool currently_recording{false};
};
