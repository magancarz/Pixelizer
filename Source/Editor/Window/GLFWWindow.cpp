#include "GLFWWindow.h"

#include <stdexcept>
#include <bit>

GLFWWindow::GLFWWindow(uint32_t width, uint32_t height, std::string name)
    : Window{width, height, std::move(name)}
{
    createWindow();
}

GLFWWindow::~GLFWWindow()
{
    glfwTerminate();
}

void GLFWWindow::createWindow()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Couldn't initialize GLFW!\n");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), window_name.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window!");
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void GLFWWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }
}

void GLFWWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto glfw_window = std::bit_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    glfw_window->framebuffer_resized = true;
    glfw_window->width = width;
    glfw_window->height = height;
}
