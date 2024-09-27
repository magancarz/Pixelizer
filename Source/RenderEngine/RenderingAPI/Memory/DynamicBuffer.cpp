#include "DynamicBuffer.h"

#include <cassert>

#include "Logs/LogSystem.h"

DynamicBuffer::DynamicBuffer(
        VkDevice logical_device,
        const CommandPool& command_pool,
        VulkanMemoryAllocator& memory_allocator,
        const BufferInfo& buffer_info)
    : Buffer(logical_device, command_pool, buffer_info, MemoryAllocationInfo{}),
    memory_allocator{memory_allocator},
    staging_buffer{memory_allocator.createStagingBuffer(buffer_info.instance_size, buffer_info.instance_count)}
{
    allocateBuffer();
}

void DynamicBuffer::allocateBuffer()
{
    allocation_info = memory_allocator.allocateBuffer(buffer_info);
}

void DynamicBuffer::write(const void* data, uint32_t instance_count)
{
    assert(instance_count > 0 && "Should this happen? Probably no");

    ensureThatBuffersAreLargeEnough(instance_count);

    staging_buffer->writeToBuffer(data);
    copyFrom(*staging_buffer);
}

void DynamicBuffer::ensureThatBuffersAreLargeEnough(uint32_t copied_data_instance_count)
{
    if (staging_buffer->getInstanceCount() < copied_data_instance_count)
    {
        staging_buffer.reset();

        buffer_info.instance_count = std::max(
            copied_data_instance_count,
            static_cast<uint32_t>(static_cast<float>(buffer_info.instance_count) * staging_buffer_instance_count_multiplier));
        staging_buffer = memory_allocator.createStagingBuffer(buffer_info.instance_size, buffer_info.instance_count);

        destroyBuffer();
        allocateBuffer();
    }
}
