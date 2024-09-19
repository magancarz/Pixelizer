#include "InputSystem.h"

InputManager& InputSystem::initialize(std::unique_ptr<InputManager> input_manager)
{
    input_imp = std::move(input_manager);
    return *input_imp;
}
