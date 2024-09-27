#include "Window.h"

Window::Window(uint32_t width, uint32_t height, std::string name)
    : width{width}, height{height}, window_name{std::move(name)} {}
