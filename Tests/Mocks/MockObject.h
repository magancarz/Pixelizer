#pragma once
#include <RenderEngine/FrameInfo.h>

class MockObject : public Object
{
public:
    MockObject() = default;

    MOCK_METHOD(void, update, (FrameInfo&), (override));
};
