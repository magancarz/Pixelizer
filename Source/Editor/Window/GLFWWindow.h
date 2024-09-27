#pragma once

#include <vulkan/vulkan_core.h>
#include "GLFW/glfw3.h"

#include "Editor/Defines.h"
#include "Window.h"

class GLFWWindow : public Window
{
public:
    explicit GLFWWindow(
        uint32_t width = Editor::DEFAULT_WINDOW_WIDTH,
        uint32_t height = Editor::DEFAULT_WINDOW_HEIGHT,
        std::string name = Editor::DEFAULT_WINDOW_TITLE);
    ~GLFWWindow() override;

    GLFWWindow(const GLFWWindow&) = delete;
    GLFWWindow& operator=(const GLFWWindow&) = delete;

    [[nodiscard]] bool shouldClose() const override { return glfwWindowShouldClose(window); }
    [[nodiscard]] GLFWwindow* getGFLWwindow() const { return window; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    void createWindow();

    GLFWwindow* window{nullptr};

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};
