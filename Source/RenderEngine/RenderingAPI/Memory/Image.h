#pragma once
#include "AllocatorInfo.h"

class Image
{
public:
    Image(VkImage image, const MemoryAllocationInfo& allocator_info);
    ~Image();

    Image(const Image&) = delete;
    Image operator=(const Image&) = delete;
    Image(const Image&&) = delete;
    Image operator=(const Image&&) = delete;

    [[nodiscard]] VkImage getImage() const { return image; }
    [[nodiscard]] MemoryAllocationInfo getAllocatorInfo() const { return allocator_info; }

private:
    VkImage image{VK_NULL_HANDLE};
    MemoryAllocationInfo allocator_info{};
};
