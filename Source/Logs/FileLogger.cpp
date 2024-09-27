#include "FileLogger.h"

#include <iostream>

FileLogger::FileLogger(const std::string& log_file_location)
    : FileLogger(nullptr, log_file_location) {}

FileLogger::FileLogger(std::unique_ptr<Logger> next_logger, const std::string& log_file_location)
    : LoggerDecorator(std::move(next_logger)), log_file(log_file_location, std::ios::out)
{
    if (!log_file.is_open())
    {
        throw std::runtime_error("Unable to open log file!");
    }
}

FileLogger::~FileLogger()
{
    if (log_file.is_open())
    {
        log_file.close();
    }
}

void FileLogger::log(LogSeverity severity, const char* message)
{
    log_file << message;

    LoggerDecorator::log(severity, message);
}
