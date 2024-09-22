#pragma once

#include <string>

#include "Assets/Texture/TextureData.h"

class Texture;

struct MaterialInfo
{
    std::string name;
    TextureData* diffuse_texture{nullptr};
};