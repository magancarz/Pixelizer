#pragma once
#include <gmock/gmock-function-mocker.h>

class MockObjectComponent : public ObjectComponent
{
public:
    explicit MockObjectComponent(Object& owner) : ObjectComponent(owner) {}

    MOCK_METHOD(void, update, (FrameInfo&), (override));
};
