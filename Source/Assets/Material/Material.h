#pragma once

#include "MaterialInfo.h"
#include "RenderEngine/Textures/Texture.h"

class VulkanMemoryAllocator;

class Material
{
public:
    explicit Material(const MaterialInfo& in_material_info);

    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] TextureData* getDiffuseTexture() const { return diffuse_texture; }

protected:
    std::string name{};

    TextureData* diffuse_texture{nullptr};
};
