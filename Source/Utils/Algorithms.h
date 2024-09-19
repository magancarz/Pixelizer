#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

namespace Algorithms
{
    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
    {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    }

    template <typename T>
    T align(T to_align, T alignment)
    {
        return glm::round(to_align / alignment) * alignment;
    }

    const float ALLOWED_PRECISION{std::exp2f(-16)};
    const float EPSILON{static_cast<float>(glm::exp2(-23))};

    bool equal(float first, float second, float precision = ALLOWED_PRECISION);
}
