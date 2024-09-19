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
    }
    else if (severity == LogSeverity::WARNING)
    {
#ifdef __linux__
        std::cout << "\033[0;33m" << message << "\033[0m";
#elif _WIN64
        GetConsoleScreenBufferInfo(h_console, &console_info);
        saved_attributes = console_info.wAttributes;

        SetConsoleTextAttribute(h_console, FOREGROUND_BLUE);
        printf("%s", message);

        SetConsoleTextAttribute(h_console, saved_attributes);
#endif
    }
    else
    {
        std::cerr << message;
    }
}
