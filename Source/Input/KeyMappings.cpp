#include "KeyMappings.h"

#include "Logs/LogSystem.h"

int KeyMappings::getKeyCodeFor(KeyCode key_code) const
{
    if (!key_mappings.contains(key_code))
    {
        LogSystem::log(LogSeverity::FATAL, "Key code for key ", key_code, " doesn't exist");
        return static_cast<int>(KeyCode::NO_KEY);
    }

    return key_mappings.at(key_code);
}
