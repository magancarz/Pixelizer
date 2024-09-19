#pragma once

#include "RenderEngine/Renderer.h"
#include "Assets/AssetManager.h"
#include "Input/InputManager.h"

class Pixelizer
{
public:
    Pixelizer();
    ~Pixelizer();

    Pixelizer(const Pixelizer&) = delete;
    Pixelizer& operator=(const Pixelizer&) = delete;

    void run();

private:
    Window& window;
    InputManager& input_manager;
    VulkanSystem& vulkan_system;

    VulkanMemoryAllocator vulkan_memory_allocator;
    AssetManager asset_manager;

    Renderer renderer;
};
