#pragma once

#include "KeyCodes.h"

class InputManager
{
public:
    virtual ~InputManager() = default;

    virtual void refresh() = 0;

    virtual bool isKeyPressed(KeyCode key_mapping) = 0;
    virtual float getMouseXAxisMovement() = 0;
    virtual float getMouseYAxisMovement() = 0;
};