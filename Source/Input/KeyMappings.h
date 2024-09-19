#pragma once

#include <unordered_map>

#include "KeyCodes.h"

class KeyMappings
{
public:
    int getKeyCodeFor(KeyCode key_code) const;

protected:
    std::unordered_map<KeyCode, int> key_mappings;
};
