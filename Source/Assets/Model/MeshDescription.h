#pragma once

#include "ModelMemoryDescription.h"

struct MeshDescription
{
    std::vector<ModelDescription> model_descriptions;
    std::vector<std::string> required_materials;
};