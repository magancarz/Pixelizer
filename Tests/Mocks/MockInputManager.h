#pragma once

#include <gmock/gmock.h>

#include "Input/InputManager.h"

class MockInputManager : public InputManager
{
public:
    MOCK_METHOD(bool, isKeyPressed, (KeyCode key_code), (override));
    MOCK_METHOD(void, refresh, (), (override));
    MOCK_METHOD(float, getMouseXAxisMovement, (), (override));
    MOCK_METHOD(float, getMouseYAxisMovement, (), (override));
};