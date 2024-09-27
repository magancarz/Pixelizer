#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"

#include "Assets/Defines.h"

struct TextureData
{
    std::string name{Assets::EMPTY_TEXTURE_NAME};
    uint32_t width{0};
    uint32_t height{0};
    uint32_t number_of_channels{4};
    std::vector<unsigned char> data;
    VkFormat format;
    uint32_t mip_levels{0};
};