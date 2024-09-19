#pragma once
#include <unordered_set>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> compute_family;
    std::optional<uint32_t> present_family;
    std::optional<uint32_t> transfer_family;

    [[nodiscard]] bool isComplete() const
    {
        return graphics_family.has_value() && compute_family.has_value() && present_family.has_value() && transfer_family.has_value();
    }

    [[nodiscard]] uint32_t uniqueIndices() const
    {
        assert(isComplete());
        std::unordered_set<uint32_t> set
        {
            graphics_family.value(),
            compute_family.value(),
            present_family.value(),
            transfer_family.value()
        };
        return set.size();
    }
};
