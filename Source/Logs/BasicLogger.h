#pragma once

#include "Logger.h"

class BasicLogger : public Logger
{
public:
    void log(LogSeverity severity, const char* message) override;
};
