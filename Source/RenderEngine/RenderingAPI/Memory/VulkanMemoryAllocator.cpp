#include "VulkanMemoryAllocator.h"

#include "DynamicBuffer.h"

VulkanMemoryAllocator::VulkanMemoryAllocator(Instance& vulkan_instance, PhysicalDevice& physical_device, Device& logical_device, const CommandPool& transfer_command_pool)
    : vulkan_instance{vulkan_instance}, physical_device{physical_device}, logical_device{logical_device}, transfer_command_pool{transfer_command_pool}
{
    initializeVMA();
}

void VulkanMemoryAllocator::initializeVMA()
{
    VmaVulkanFunctions vulkan_functions{};
    vulkan_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkan_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocator_create_info{};
    allocator_create_info.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_3;
    allocator_create_info.instance = vulkan_instance.getInstance();
    allocator_create_info.physicalDevice = physical_device.getPhysicalDevice();
    allocator_create_info.device = logical_device.handle();
    allocator_create_info.pVulkanFunctions = &vulkan_functions;

    vmaCreateAllocator(&allocator_create_info, &allocator);
}

VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
    vmaDestroyAllocator(allocator);
}

VkDeviceSize VulkanMemoryAllocator::getAlignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment)
{
    if (min_offset_alignment > 0)
    {
        return (instance_size + min_offset_alignment - 1) & ~(min_offset_alignment - 1);
    }
    return instance_size;
}

std::unique_ptr<Buffer> VulkanMemoryAllocator::createBuffer(const BufferInfo& buffer_info)
{
    MemoryAllocationInfo memory_allocation_info = allocateBuffer(buffer_info);

    return std::make_unique<Buffer>(
        logical_device.handle(),
        transfer_command_pool,
        buffer_info,
        memory_allocation_info);
}

MemoryAllocationInfo VulkanMemoryAllocator::allocateBuffer(const BufferInfo& buffer_info)
{
    VkDeviceSize alignment_size = getAlignment(buffer_info.instance_size, buffer_info.min_offset_alignment);
    auto buffer_size = static_cast<uint32_t>(alignment_size * buffer_info.instance_count);

    VkBufferCreateInfo buffer_create_info{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_create_info.size = buffer_size;
    buffer_create_info.usage = buffer_info.usage_flags;
    buffer_create_info.pNext = nullptr;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_info.requiredFlags = buffer_info.required_memory_flags;
    alloc_info.preferredFlags = buffer_info.preferred_memory_flags;
    alloc_info.flags = buffer_info.allocation_flags;

    MemoryAllocationInfo memory_allocation_info{};
    memory_allocation_info.vma_allocator = allocator;
    memory_allocation_info.buffer_size = buffer_size;

    vmaCreateBuffer(allocator, &buffer_create_info, &alloc_info, &memory_allocation_info.buffer, &memory_allocation_info.vma_allocation, &memory_allocation_info.vma_allocation_info);

    return memory_allocation_info;
}

std::unique_ptr<DynamicBuffer> VulkanMemoryAllocator::createDeviceBuffer(const BufferInfo& buffer_info)
{
    return std::make_unique<DynamicBuffer>(
        logical_device.handle(),
        transfer_command_pool,
        *this,
        buffer_info);
}

std::unique_ptr<Buffer> VulkanMemoryAllocator::createStagingBuffer(uint32_t instance_size, uint32_t instance_count)
{
    BufferInfo buffer_info = createStagingBufferInfo(instance_size, instance_count);

    return createBuffer(buffer_info);
}

BufferInfo VulkanMemoryAllocator::createStagingBufferInfo(uint32_t instance_size, uint32_t instance_count)
{
    BufferInfo buffer_info{};
    buffer_info.instance_size = instance_size;
    buffer_info.instance_count = instance_count;
    buffer_info.usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_info.required_memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer_info.allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    return buffer_info;
}

std::unique_ptr<Buffer> VulkanMemoryAllocator::createStagingBuffer(uint32_t instance_size, uint32_t instance_count, const void* data)
{
    BufferInfo buffer_info = createStagingBufferInfo(instance_size, instance_count);

    auto buffer = createBuffer(buffer_info);
    buffer->map();
    buffer->writeToBuffer(data);
    buffer->unmap();

    return buffer;
}

std::unique_ptr<Image> VulkanMemoryAllocator::createImage(const VkImageCreateInfo& image_create_info)
{
    VmaAllocationCreateInfo alloc_create_info{};
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_create_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    alloc_create_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    alloc_create_info.priority = 1.0f;

    VkImage image;
    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;
    vmaCreateImage(allocator, &image_create_info, &alloc_create_info, &image, &allocation, &allocation_info);

    MemoryAllocationInfo allocator_info{};
    allocator_info.vma_allocation = allocation;
    allocator_info.vma_allocation_info = allocation_info;
    allocator_info.vma_allocator = allocator;
    return std::make_unique<Image>(image, allocator_info);
}
