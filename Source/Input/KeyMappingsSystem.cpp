#include "KeyMappingsSystem.h"

void KeyMappingsSystem::initialize(std::unique_ptr<KeyMappings> key_mappings)
{
    key_mappings_impl = std::move(key_mappings);
}

int KeyMappingsSystem::getImplKeyCodeFor(KeyCode key_code)
{
    return key_mappings_impl->getKeyCodeFor(key_code);
}
