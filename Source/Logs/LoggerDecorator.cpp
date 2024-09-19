#include "LoggerDecorator.h"

LoggerDecorator::LoggerDecorator(std::unique_ptr<Logger> wrapee)
    : wrapee{std::move(wrapee)} {}

void LoggerDecorator::log(LogSeverity severity, const char* message)
{
    if (wrapee)
    {
        wrapee->log(severity, message);
    }
}
