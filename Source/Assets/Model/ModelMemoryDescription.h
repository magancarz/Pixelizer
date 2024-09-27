#pragma once

#include "RenderEngine/RenderingAPI/Memory/Buffer.h"

struct ModelDescription
{
    Buffer* vertex_buffer{nullptr};
    Buffer* index_buffer{nullptr};
    uint64_t num_of_triangles{0};
    uint64_t num_of_vertices{0};
    uint64_t num_of_indices{0};
};