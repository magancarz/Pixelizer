#pragma once

#include <memory>

#include "KeyMappings.h"

class KeyMappingsSystem
{
public:
    static void initialize(std::unique_ptr<KeyMappings> key_mappings);

    static int getImplKeyCodeFor(KeyCode key_code);

private:
    inline static std::unique_ptr<KeyMappings> key_mappings_impl{};
};