#include "Pixelizer.h"

#include "Logs/BasicLogger.h"
#include "Logs/Defines.h"
#include "Logs/FileLogger.h"

#include "Logs/LogSystem.h"
#include "Utils/PathBuilder.h"

void initializeLogSystem()
{
    auto basic_logger = std::make_unique<BasicLogger>();
    std::string log_file_location = PathBuilder().append(Logs::LOG_FOLDER_LOCATION).append(Logs::DEFAULT_LOG_FILE_NAME).build();
    auto file_logger_decorator = std::make_unique<FileLogger>(std::move(basic_logger), log_file_location);
    LogSystem::initialize(std::move(file_logger_decorator));
}

int main()
{
    initializeLogSystem();

    Pixelizer app{};

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
