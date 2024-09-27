#pragma once

#include <memory>

#include "Window.h"

class WindowSystem
{
public:
    static Window& initialize(std::unique_ptr<Window> window);

    [[nodiscard]] static Window& get() { return *window_impl; }

private:
    inline static std::unique_ptr<Window> window_impl;
};
