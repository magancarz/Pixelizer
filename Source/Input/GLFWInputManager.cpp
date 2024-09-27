#include "GLFWInputManager.h"

#include "GLFWKeyMappings.h"
#include "KeyMappingsSystem.h"
#include "Editor/Window/GLFWWindow.h"
#include "Editor/Window/WindowSystem.h"
#include "Logs/LogSeverity.h"
#include "Logs/LogSystem.h"

GLFWInputManager::GLFWInputManager()
{
    obtainGLFWwindowPointer();
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    KeyMappingsSystem::initialize(std::make_unique<GLFWKeyMappings>());
}

void GLFWInputManager::obtainGLFWwindowPointer()
{
    if (const auto as_glfw_window = dynamic_cast<GLFWWindow*>(&WindowSystem::get()))
    {
        window = as_glfw_window->getGFLWwindow();
    }
    else
    {
        LogSystem::log(LogSeverity::FATAL, "Couldn't obtain glfw window pointer!");
    }
}

void GLFWInputManager::refresh()
{
    glfwPollEvents();
    refreshMouseMovementStatus();
}

bool GLFWInputManager::isKeyPressed(KeyCode key_mapping)
{
    return glfwGetKey(window, KeyMappingsSystem::getImplKeyCodeFor(key_mapping)) == GLFW_PRESS;
}

void GLFWInputManager::refreshMouseMovementStatus()
{
    static bool first_mouse{true};

    double x_pos;
    double y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);

    if (first_mouse)
    {
        last_mouse_x = static_cast<float>(x_pos);
        last_mouse_y = static_cast<float>(y_pos);
        first_mouse = false;
    }

    mouse_offset_x = static_cast<float>(x_pos) - last_mouse_x;
    mouse_offset_y = static_cast<float>(y_pos) - last_mouse_y;

    last_mouse_x = static_cast<float>(x_pos);
    last_mouse_y = static_cast<float>(y_pos);
}

float GLFWInputManager::getMouseXAxisMovement()
{
    return mouse_offset_x;
}

float GLFWInputManager::getMouseYAxisMovement()
{
    return mouse_offset_y;
}
