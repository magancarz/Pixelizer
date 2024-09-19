#include "LogSystem.h"

void LogSystem::initialize(std::unique_ptr<Logger> logger)
{
    app_logger = std::move(logger);
}
