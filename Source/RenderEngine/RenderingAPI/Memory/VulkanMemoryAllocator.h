#pragma once

#include <memory>

#include "BufferInfo.h"
#include "VmaUsage.h"
#include "Image.h"

class CommandPool;
class Device;
class PhysicalDevice;
class Instance;
class Buffer;
class DynamicBuffer;

class VulkanMemoryAllocator
{
public:
    VulkanMemoryAllocator(Instance& vulkan_instance, PhysicalDevice& physical_device, Device& logical_device, const CommandPool& transfer_command_pool);
    ~VulkanMemoryAllocator();

    VulkanMemoryAllocator(const VulkanMemoryAllocator&) = delete;
    VulkanMemoryAllocator& operator=(const VulkanMemoryAllocator&) = delete;
    VulkanMemoryAllocator(VulkanMemoryAllocator&&) = default;
    VulkanMemoryAllocator& operator=(VulkanMemoryAllocator&&) = delete;

    std::unique_ptr<Buffer> createBuffer(const BufferInfo& buffer_info);
    std::unique_ptr<DynamicBuffer> createDeviceBuffer(const BufferInfo& buffer_info);
    std::unique_ptr<Buffer> createStagingBuffer(uint32_t instance_size, uint32_t instance_count);
    std::unique_ptr<Buffer> createStagingBuffer(uint32_t instance_size, uint32_t instance_count, const void *data);
    std::unique_ptr<Image> createImage(const VkImageCreateInfo& image_create_info);

    MemoryAllocationInfo allocateBuffer(const BufferInfo& buffer_info);

private:
    Instance& vulkan_instance;
    PhysicalDevice& physical_device; 
    Device& logical_device;
    const CommandPool& transfer_command_pool;

    void initializeVMA();

    VmaAllocator allocator{VK_NULL_HANDLE};

    static VkDeviceSize getAlignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);
    BufferInfo createStagingBufferInfo(uint32_t instance_size, uint32_t instance_count);
};
