#include "Surface.h"
#include "Editor/Window/WindowSystem.h"
#include "VulkanDefines.h"
#include "Editor/Window/GLFWWindow.h"
#include "Logs/LogSystem.h"

Surface::Surface(Instance& instance)
    : instance{instance}
{
    createSurface();
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(instance.getInstance(), surface, VulkanDefines::NO_CALLBACK);
}

void Surface::createSurface()
{
    if (auto as_glfw_window = dynamic_cast<GLFWWindow*>(&WindowSystem::get()))
    {
        as_glfw_window->createWindowSurface(instance.getInstance(), &surface);
    }
    else
    {
        LogSystem::log(LogSeverity::FATAL, "Unable to fetch glfw window!");
    }
}