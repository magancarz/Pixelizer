#pragma once

#include <gmock/gmock.h>

#include "Logs/Logger.h"

class MockLogger : public Logger
{
public:
    MOCK_METHOD(void, log, (LogSeverity, const char*), (override));
};