#pragma once

#include "Buffer.h"
#include "VulkanMemoryAllocator.h"

class DynamicBuffer : public Buffer
{
public:
    DynamicBuffer(
        VkDevice logical_device,
        const CommandPool& command_pool,
        VulkanMemoryAllocator& memory_allocator,
        const BufferInfo& buffer_info);

    DynamicBuffer(const DynamicBuffer&) = delete;
    DynamicBuffer& operator=(const DynamicBuffer&) = delete;

    void write(const void* data, uint32_t instance_count);

private:
    VulkanMemoryAllocator& memory_allocator;

    void allocateBuffer();

    void ensureThatBuffersAreLargeEnough(uint32_t copied_data_instance_count);

    std::unique_ptr<Buffer> staging_buffer;
    float staging_buffer_instance_count_multiplier{1.5f};
};
