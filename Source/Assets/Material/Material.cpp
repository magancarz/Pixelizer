#include "Material.h"

Material::Material(const MaterialInfo& material_info)
    : name{material_info.name},
    diffuse_texture{material_info.diffuse_texture} {}
