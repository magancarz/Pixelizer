#pragma once

#include "LogSeverity.h"

class Logger
{
public:
    virtual ~Logger() = default;

    virtual void log(LogSeverity severity, const char* message) = 0;
};
