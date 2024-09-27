#pragma once

#include <string>
#include <vector>

#include "Material/MaterialData.h"
#include "Model/ModelData.h"

struct MeshData
{
    std::string name{Assets::EMPTY_MESH_NAME};
    std::vector<ModelData> models_data;
    std::vector<MaterialData> materials_data;
};
