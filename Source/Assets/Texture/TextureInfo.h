#pragma once

#include "Assets/Defines.h"

struct TextureInfo
{
    std::string name{Assets::EMPTY_TEXTURE_NAME};
    uint32_t width{0};
    uint32_t height{0};
    uint32_t number_of_channels{4};
};
