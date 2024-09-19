#include "Pixelizer.h"

#include <chrono>

#include "Editor/Window/GLFWWindow.h"
#include "Editor/Window/WindowSystem.h"
#include "Input/GLFWInputManager.h"
#include "Input/InputSystem.h"
#include "RenderEngine/RenderingAPI/Memory/VulkanMemoryAllocator.h"

Pixelizer::Pixelizer()
    : window{WindowSystem::initialize(std::make_unique<GLFWWindow>())},
    input_manager{InputSystem::initialize(std::make_unique<GLFWInputManager>())},
    vulkan_system{VulkanSystem::initialize()},
    vulkan_memory_allocator{vulkan_system.getInstance(), vulkan_system.getPhysicalDevice(), vulkan_system.getLogicalDevice(), vulkan_system.getTransferCommandPool()},
    asset_manager{vulkan_system, vulkan_memory_allocator},
    renderer{window, vulkan_system.getSurface(), vulkan_system.getPhysicalDevice(), vulkan_system.getLogicalDevice(), vulkan_system.getGraphicsCommandPool(),
        vulkan_memory_allocator, asset_manager} {}

Pixelizer::~Pixelizer()
{
    vkDeviceWaitIdle(vulkan_system.getLogicalDevice().handle());
}

void Pixelizer::run()
{
    auto last_time = std::chrono::high_resolution_clock::now();
    while (!window.shouldClose())
    {
        input_manager.refresh();

        FrameInfo frame_info{};

        auto current_time = std::chrono::high_resolution_clock::now();
        frame_info.delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;

        renderer.render(frame_info);
    }
}