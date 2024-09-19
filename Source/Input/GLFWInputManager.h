#pragma once

#include <GLFW/glfw3.h>

#include "InputManager.h"

class GLFWInputManager : public InputManager
{
public:
    GLFWInputManager();

    GLFWInputManager(const GLFWInputManager&) = delete;
    GLFWInputManager operator=(const GLFWInputManager&) = delete;

    void refresh() override;

    bool isKeyPressed(KeyCode key_mapping) override;
    float getMouseXAxisMovement() override;
    float getMouseYAxisMovement() override;

private:
    void obtainGLFWwindowPointer();

    GLFWwindow* window{nullptr};

    void refreshMouseMovementStatus();

    float last_mouse_x{0};
    float last_mouse_y{0};
    float mouse_offset_x{0};
    float mouse_offset_y{0};
};