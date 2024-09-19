#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <mutex>

#include "Logger.h"
#include "LogSeverity.h"

class LogSystem
{
public:
    static void initialize(std::unique_ptr<Logger> logger);

    template<typename... Args>
    static void log(LogSeverity severity, Args... args)
    {
        if (!app_logger)
        {
            std::cout << "Cannot log if application logger is not properly initialized!\n";
            return;
        }
        std::lock_guard lock_guard(log_mutex);

        std::ostringstream oss;
        oss << LOG_SEVERITY_PREFIXES.at(severity);
        format(oss, args...);
        oss << '\n';

        const std::string message = oss.str();
        app_logger->log(severity, message.c_str());
    }

    inline static std::unordered_map<LogSeverity, std::string> LOG_SEVERITY_PREFIXES
    {
        {LogSeverity::VERBOSE, "Verbose: "},
        {LogSeverity::LOG, "Log: "},
        {LogSeverity::WARNING, "Warning: "},
        {LogSeverity::ERROR, "Error: "},
        {LogSeverity::FATAL, "Fatal: "}
    };

private:
    template<typename T>
    static void format(std::ostringstream& oss, T t)
    {
        oss << t;
    }

    template<typename T, typename... Args>
    static void format(std::ostringstream& oss, T t, Args... args)
    {
        oss << t;
        format(oss, args...);
    }

    inline static std::unique_ptr<Logger> app_logger;
    inline static std::mutex log_mutex;
};
