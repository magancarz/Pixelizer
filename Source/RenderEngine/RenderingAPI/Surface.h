#pragma once

#include "Instance.h"

class Surface
{
public:
    explicit Surface(Instance& instance);
    ~Surface();

    [[nodiscard]] VkSurfaceKHR getSurface() const { return surface; }

private:
    void createSurface();

    Instance& instance;
    VkSurfaceKHR surface{VK_NULL_HANDLE};
};
