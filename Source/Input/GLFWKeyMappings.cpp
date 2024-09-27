#include "GLFWKeyMappings.h"

#include "GLFW/glfw3.h"

GLFWKeyMappings::GLFWKeyMappings()
{
    key_mappings.emplace(KeyCode::MOVE_LEFT, GLFW_KEY_A);
    key_mappings.emplace(KeyCode::MOVE_RIGHT, GLFW_KEY_D);
    key_mappings.emplace(KeyCode::MOVE_FORWARD, GLFW_KEY_W);
    key_mappings.emplace(KeyCode::MOVE_BACKWARD, GLFW_KEY_S);
    key_mappings.emplace(KeyCode::MOVE_UP, GLFW_KEY_E);
    key_mappings.emplace(KeyCode::MOVE_DOWN, GLFW_KEY_Q);
    key_mappings.emplace(KeyCode::LOOK_LEFT, GLFW_KEY_LEFT);
    key_mappings.emplace(KeyCode::LOOK_RIGHT, GLFW_KEY_RIGHT);
    key_mappings.emplace(KeyCode::LOOK_UP, GLFW_KEY_UP);
    key_mappings.emplace(KeyCode::LOOK_DOWN, GLFW_KEY_DOWN);
    key_mappings.emplace(KeyCode::JUMP, GLFW_KEY_SPACE);
}
