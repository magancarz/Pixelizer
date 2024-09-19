#pragma once

#include <memory>

#include "InputManager.h"

class InputSystem
{
public:
    static InputManager& initialize(std::unique_ptr<InputManager> input_manager);

    [[nodiscard]] static InputManager& get() { return *input_imp; }

private:
    inline static std::unique_ptr<InputManager> input_imp;
};
