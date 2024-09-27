#pragma once

#include <memory>

#include "Logger.h"

class LoggerDecorator : public Logger
{
public:
    explicit LoggerDecorator(std::unique_ptr<Logger> wrapee);

    void log(LogSeverity severity, const char* message) override;

private:
    std::unique_ptr<Logger> wrapee;
};