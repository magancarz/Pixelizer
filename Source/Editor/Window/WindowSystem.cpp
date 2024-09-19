#include "WindowSystem.h"

Window& WindowSystem::initialize(std::unique_ptr<Window> window)
{
    window_impl = std::move(window);
    return *window_impl;
}
