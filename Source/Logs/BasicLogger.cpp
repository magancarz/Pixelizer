#include "BasicLogger.h"

#include <iostream>

#ifdef _WIN64
#include <windows.h>
#endif

void BasicLogger::log(LogSeverity severity, const char* message)
{
#ifdef _WIN64
    HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO console_info;
    WORD saved_attributes;
#endif

    if (severity == LogSeverity::VERBOSE || severity == LogSeverity::LOG)
    {
        std::cout << message;
        return;
    }

    if (severity == LogSeverity::WARNING)
    {
#ifdef __linux__
        std::cout << "\o{33}[0;33m" << message << "\o{33}[0m";
#elif _WIN64
        GetConsoleScreenBufferInfo(h_console, &console_info);
        saved_attributes = console_info.wAttributes;

        SetConsoleTextAttribute(h_console, FOREGROUND_BLUE);
        printf("%s", message);

        SetConsoleTextAttribute(h_console, saved_attributes);
#endif
        return;
    }

    std::cerr << message;
}
