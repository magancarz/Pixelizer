#include "TextureLoader.h"

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Assets/Defines.h"
#include "Logs/LogSystem.h"
#include "Utils/PathBuilder.h"
#include "RenderEngine/RenderingAPI/Textures/Texture.h"

TextureData TextureLoader::loadFromAssetFile(const std::string& texture_name)
{
    const std::string location = PathBuilder().append(Assets::TEXTURES_DIRECTORY_PATH.string()).append(texture_name).build();

    int width;
    int height;
    int number_of_channels;
    unsigned char* data = stbi_load(location.c_str(), &width, &height, &number_of_channels, EXPECTED_NUMBER_OF_CHANNELS);

    if (!data)
    {
        LogSystem::log(LogSeverity::ERROR, "Failed to load image ", location, "! Returning empty image...");
        return TextureData{};
    }

    if (number_of_channels != EXPECTED_NUMBER_OF_CHANNELS)
    {
        LogSystem::log(LogSeverity::WARNING, "Encountered different number of channels than expected with texture ", location, "!");
    }

    std::vector<unsigned char> copied_data(width * height * EXPECTED_NUMBER_OF_CHANNELS);
    memcpy(copied_data.data(), data, copied_data.size() * sizeof(unsigned char));
    stbi_image_free(data);

    return TextureData
    {
        .name = texture_name,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .number_of_channels = static_cast<uint32_t>(EXPECTED_NUMBER_OF_CHANNELS),
        .data = std::move(copied_data),
        .mip_levels = 1
    };
}
