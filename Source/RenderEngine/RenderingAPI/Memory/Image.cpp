#include "Image.h"

Image::Image(VkImage image, const MemoryAllocationInfo& allocator_info)
    : image{image}, allocator_info{allocator_info}
{
}

Image::~Image()
{
    vmaDestroyImage(allocator_info.vma_allocator, image, allocator_info.vma_allocation);
}
