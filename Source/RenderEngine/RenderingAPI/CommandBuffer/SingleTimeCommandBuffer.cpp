#include "SingleTimeCommandBuffer.h"

SingleTimeCommandBuffer::SingleTimeCommandBuffer(const Device& logical_device, const CommandPool& owning_command_pool, const Queue& queue, VkCommandBuffer command_buffer)
    : owning_command_pool{owning_command_pool}, queue{queue}, command_buffer{command_buffer}, fence{logical_device} {}

SingleTimeCommandBuffer::~SingleTimeCommandBuffer()
{
    assert(!currently_recording);
    owning_command_pool.freeCommandBufferHandles(1u, &command_buffer);
}

VkCommandBuffer SingleTimeCommandBuffer::beginRecording()
{
    assert(!currently_recording);
    fence.reset();

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    owning_command_pool.beginCommandBuffer(command_buffer, begin_info);
    currently_recording = true;

    return command_buffer;
}

const Fence& SingleTimeCommandBuffer::endRecordingAndSubmit()
{
    assert(currently_recording);

    owning_command_pool.endCommandBuffer(command_buffer);
    currently_recording = false;

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    queue.submitCommandBuffer(1u, &submit_info, fence.handle());

    return fence;
}
