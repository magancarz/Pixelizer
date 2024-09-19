#include "TextureData.h"

glm::vec3 TextureData::sample(const glm::vec2& uv)
{
    glm::vec2 corrected_uv = uv;

    while (corrected_uv.x >= 1.0f)
    {
        corrected_uv.x -= 1.0f;
    }

    while (corrected_uv.x < 0.0f)
    {
        corrected_uv.x += 1.0f;
    }

    while (corrected_uv.y >= 1.0f)
    {
        corrected_uv.y -= 1.0f;
    }

    while (corrected_uv.y < 0.0f)
    {
        corrected_uv.y += 1.0f;
    }

    auto x_index = static_cast<uint32_t>(corrected_uv.x * static_cast<float>(width));
    auto y_index = static_cast<uint32_t>(corrected_uv.y * static_cast<float>(height));

    auto index = (y_index * width + x_index) * number_of_channels;
    return glm::vec3{data[index + 0], data[index + 1], data[index + 2]} / 255.0f;
}
