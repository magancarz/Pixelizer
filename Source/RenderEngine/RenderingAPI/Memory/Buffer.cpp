#include "Buffer.h"

#include <cassert>
#include <cstring>

#include <vulkan/vulkan_core.h>

#include "Logs/LogSystem.h"
#include "RenderEngine/RenderingAPI/CommandBuffer/SingleTimeCommandBuffer.h"

Buffer::Buffer(
        VkDevice logical_device,
        const CommandPool& command_pool,
        const BufferInfo& buffer_info,
        const MemoryAllocationInfo& allocation_info)
    : logical_device{logical_device},
    command_pool{command_pool},
    allocation_info{allocation_info},
    buffer_info{buffer_info} {}

Buffer::~Buffer()
{
    unmap();
    destroyBuffer();
}

void Buffer::destroyBuffer()
{
    vmaDestroyBuffer(allocation_info.vma_allocator, allocation_info.buffer, allocation_info.vma_allocation);
}

VkResult Buffer::map()
{
    return vmaMapMemory(allocation_info.vma_allocator, allocation_info.vma_allocation, &mapped);
}

void Buffer::unmap()
{
    if (mapped)
    {
        vmaUnmapMemory(allocation_info.vma_allocator, allocation_info.vma_allocation);
        mapped = nullptr;
    }
}

void Buffer::writeToBuffer(const void* data)
{
    writeToBuffer(data, allocation_info.buffer_size);
}

void Buffer::writeToBuffer(const void* data, size_t size, size_t offset)
{
    if (!mapped)
    {
        LogSystem::log(LogSeverity::WARNING, "Tried to copy to unmapped buffer! Mapping...");
        map();
    }

    char* mapped_as_char_ptr = static_cast<char*>(mapped);
    assert(mapped_as_char_ptr);
    memcpy(mapped_as_char_ptr + offset, data, size);
}

void Buffer::copyFrom(const Buffer& src_buffer) const
{
    SingleTimeCommandBuffer single_time_command_buffer = command_pool.createSingleTimeCommandBuffer();
    VkCommandBuffer command_buffer = single_time_command_buffer.beginRecording();

    VkBufferCopy copy_region{};
    copy_region.size = src_buffer.allocation_info.buffer_size;
    vkCmdCopyBuffer(command_buffer, src_buffer.getBuffer(), allocation_info.buffer, 1, &copy_region);

    const Fence& fence = single_time_command_buffer.endRecordingAndSubmit();
    fence.wait();
}

VkDescriptorBufferInfo Buffer::descriptorInfo() const
{
    return VkDescriptorBufferInfo
    {
        allocation_info.buffer,
        0,
        VK_WHOLE_SIZE,
    };
}

VkDeviceAddress Buffer::getBufferDeviceAddress() const
{
    if (allocation_info.buffer == VK_NULL_HANDLE)
    {
        return 0;
    }

    VkBufferDeviceAddressInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.buffer = allocation_info.buffer;

    return vkGetBufferDeviceAddress(logical_device, &info);
}
