#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 uv{};
    glm::vec3 tangent{};

    bool operator==(const Vertex& other) const
    {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};