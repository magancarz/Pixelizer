#include "gtest/gtest.h"

#include "Logs/LogSystem.h"

#include "Environment.h"
#include "Mocks/MockLogger.h"

struct LogSystemTests : public ::testing::Test
{
    static void TearDownTestSuite()
    {
        TestsEnvironment::initializeTestsLogger();
    }
};

TEST_F(LogSystemTests, shouldInitializeApplicationLogger)
{
    // given
    auto mock_logger = std::make_unique<MockLogger>();

    // when & then
    LogSystem::initialize(std::move(mock_logger));
}

TEST_F(LogSystemTests, shouldLogMessage)
{
    // given
    auto mock_logger = std::make_unique<MockLogger>();

    // then
    LogSeverity expected_severity = LogSeverity::LOG;
    std::string first_part = "first";
    std::string second_part = "second";
    std::string expected_log = LogSystem::LOG_SEVERITY_PREFIXES.at(expected_severity) + first_part + " " + second_part + '\n';
    EXPECT_CALL(*mock_logger, log(expected_severity, testing::StrEq(expected_log))).Times(1);

    // given
    LogSystem::initialize(std::move(mock_logger));

    // when
    LogSystem::log(expected_severity, first_part, " ", second_part);
}