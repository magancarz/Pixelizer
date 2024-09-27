#pragma once

#include <string>

#include <Assets/Defines.h>

struct MaterialData
{
    std::string name{Assets::DEFAULT_MATERIAL_NAME};
    std::string diffuse_texture_name{Assets::DEFAULT_DIFFUSE_TEXTURE};
};
