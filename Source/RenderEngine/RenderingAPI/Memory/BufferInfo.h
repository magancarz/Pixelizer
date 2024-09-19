#pragma once

struct BufferInfo
{
    uint32_t instance_size;
    uint32_t instance_count;
    uint32_t usage_flags;
    uint32_t required_memory_flags;
    uint32_t allocation_flags;
    uint32_t preferred_memory_flags;
    uint32_t min_offset_alignment;
};