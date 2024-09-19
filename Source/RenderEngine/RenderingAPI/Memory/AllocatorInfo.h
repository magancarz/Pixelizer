#pragma once

#include "VmaUsage.h"

struct MemoryAllocationInfo
{
    VmaAllocator vma_allocator;
    VmaAllocation vma_allocation;
    VmaAllocationInfo vma_allocation_info;
    VkBuffer buffer;
    uint32_t buffer_size;
};
