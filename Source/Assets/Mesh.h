#pragma once

#include <string>
#include <vector>

#include "Assets/Model/Model.h"
#include "Assets/Material/Material.h"

struct Mesh
{
    std::string name;
    std::vector<Model*> models;
    std::vector<Material*> materials;
};