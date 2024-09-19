#include "CommandBuffer.h"

#include "CommandPool.h"

CommandBuffer::CommandBuffer(const CommandPool& owning_command_pool, VkCommandBuffer command_buffer)
    : owning_command_pool{owning_command_pool}, command_buffer{command_buffer} {}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : owning_command_pool{other.owning_command_pool}, command_buffer{other.command_buffer}
{
    other.command_buffer = VK_NULL_HANDLE;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
    command_buffer = other.command_buffer;
    other.command_buffer = VK_NULL_HANDLE;
    return *this;
}

CommandBuffer::~CommandBuffer()
{
    assert(!currently_recording);
    if (command_buffer != VK_NULL_HANDLE)
    {
        owning_command_pool.freeCommandBufferHandles(1u, &command_buffer);
    }
}

void CommandBuffer::invalidate()
{
    command_buffer = VK_NULL_HANDLE;
}

VkCommandBuffer CommandBuffer::beginRecording()
{
    assert(!currently_recording);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    owning_command_pool.beginCommandBuffer(command_buffer, begin_info);
    currently_recording = true;

    return command_buffer;
}

void CommandBuffer::endRecording()
{
    assert(currently_recording);

    owning_command_pool.endCommandBuffer(command_buffer);
    currently_recording = false;
}

void CommandBuffer::reset()
{
    assert(!currently_recording);

    owning_command_pool.resetCommandBuffer(command_buffer);
}
