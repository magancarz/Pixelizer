#include "gtest/gtest.h"

#include <Logs/BasicLogger.h>

TEST(BasicLoggerTests, shouldLogMessageWithLogSeverity)
{
    // given
    BasicLogger basic_logger{};

    // when & then
    basic_logger.log(LogSeverity::LOG, "dummy log");
}

TEST(BasicLoggerTests, shouldLogMessageWithWarningSeverity)
{
    // given
    BasicLogger basic_logger{};

    // when & then
    basic_logger.log(LogSeverity::WARNING, "dummy warning");
}

TEST(BasicLoggerTests, shouldLogMessageWithErrorSeverity)
{
    // given
    BasicLogger basic_logger{};

    // when & then
    basic_logger.log(LogSeverity::ERROR, "dummy error");
}

TEST(BasicLoggerTests, shouldLogMessageWithFatalSeverity)
{
    // given
    BasicLogger basic_logger{};

    // when & then
    basic_logger.log(LogSeverity::FATAL, "dummy fatal error");
}