#pragma once

#include "RenderEngine/RenderingAPI/VulkanSystem.h"
#include "AllocatorInfo.h"
#include "BufferInfo.h"

class Buffer
{
public:
    Buffer(
        VkDevice logical_device,
        const CommandPool& command_pool,
        const BufferInfo& buffer_info,
        const MemoryAllocationInfo& allocation_info);
    virtual ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    VkResult map();
    void unmap();

    void writeToBuffer(const void* data);
    void writeToBuffer(const void* data, size_t size, size_t offset = 0);
    void copyFrom(const Buffer& src_buffer) const;

    [[nodiscard]] VkDescriptorBufferInfo descriptorInfo() const;

    [[nodiscard]] VkBuffer getBuffer() const { return allocation_info.buffer; }
    [[nodiscard]] uint32_t getSize() const { return allocation_info.buffer_size; }
    [[nodiscard]] uint32_t getInstanceCount() const { return buffer_info.instance_count; }
    [[nodiscard]] uint32_t getInstanceSize() const { return buffer_info.instance_size; }
    [[nodiscard]] void* getMappedMemory() const { return mapped; }
    [[nodiscard]] VkDeviceAddress getBufferDeviceAddress() const;

protected:
    VkDevice logical_device;
    const CommandPool& command_pool;

    void destroyBuffer();

    MemoryAllocationInfo allocation_info;
    BufferInfo buffer_info;

    void* mapped{nullptr};
};
