#pragma once

#include <string>

#include <vulkan/vulkan_core.h>

#include "Editor/Defines.h"

class Window
{
public:
    explicit Window(
        uint32_t width = Editor::DEFAULT_WINDOW_WIDTH,
        uint32_t height = Editor::DEFAULT_WINDOW_HEIGHT,
        std::string name = Editor::DEFAULT_WINDOW_TITLE);
    virtual ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    [[nodiscard]] virtual bool shouldClose() const = 0;
    [[nodiscard]] VkExtent2D getExtent() const { return {width, height}; }
    [[nodiscard]] float getAspect() const { return static_cast<float>(width) / static_cast<float>(height); }

    [[nodiscard]] bool wasWindowResized() const { return framebuffer_resized; }
    void resetWindowResizedFlag() { framebuffer_resized = false; }

protected:
    uint32_t width;
    uint32_t height;
    std::string window_name;

    bool framebuffer_resized{false};
};
