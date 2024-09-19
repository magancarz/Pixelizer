#include "VeraMaterialLoader.h"

#include <fstream>

#include "Assets/Defines.h"
#include "Logs/LogSystem.h"
#include "Utils/PathBuilder.h"
#include "MaterialData.h"

MaterialData VeraMaterialLoader::loadAssetFromFile(const std::string& asset_name)
{
    LogSystem::log(LogSeverity::LOG, "Trying to load material ", asset_name.c_str(), "...");

    const std::string filepath = PathBuilder().append(Assets::MATERIALS_DIRECTORY_PATH.string()).append(asset_name).fileExtension(Pixelizer_MATERIAL_FILE_EXTENSION).build();
    std::ifstream file_stream(filepath);
    std::string input;

    if (file_stream.is_open() && getline(file_stream, input))
    {
        MaterialData material_data{};

        std::stringstream iss(input);
        iss >> material_data.name;
        iss >> material_data.diffuse_texture_name;
        iss >> material_data.normal_map_name;

        file_stream.close();

        LogSystem::log(LogSeverity::LOG, "Loading material ", asset_name.c_str(), " ended in success");
        return material_data;
    }

    LogSystem::log(LogSeverity::ERROR, "Failed to load material ", asset_name.c_str(), ". Returning default material...");
    return MaterialData{};
}
