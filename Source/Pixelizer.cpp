#include "Pixelizer.h"

#include <chrono>

#include "Editor/Window/GLFWWindow.h"
#include "Editor/Window/WindowSystem.h"
#include "Input/GLFWInputManager.h"
#include "Input/InputSystem.h"
#include "Objects/Components/CameraComponent.h"
#include "Objects/Components/TransformComponent.h"
#include "Objects/Components/ViewerMovementComponent.h"
#include "RenderEngine/RenderingAPI/Memory/VulkanMemoryAllocator.h"

Pixelizer::Pixelizer()
    : window{WindowSystem::initialize(std::make_unique<GLFWWindow>())},
    input_manager{InputSystem::initialize(std::make_unique<GLFWInputManager>())},
    vulkan_system{VulkanSystem::initialize()},
    vulkan_memory_allocator{vulkan_system.getInstance(), vulkan_system.getPhysicalDevice(), vulkan_system.getLogicalDevice(), vulkan_system.getTransferCommandPool()},
    asset_manager{vulkan_system, vulkan_memory_allocator},
    renderer{window, vulkan_system.getSurface(), vulkan_system.getPhysicalDevice(), vulkan_system.getLogicalDevice(), vulkan_system.getGraphicsCommandPool(),
        vulkan_memory_allocator, asset_manager}
{
    prepareScene();
}

void Pixelizer::prepareScene()
{
    auto transform_component = std::make_unique<TransformComponent>(viewer_object);
    transform_component->setLocation(glm::vec3{0.0f, 0.0f, -10.0f});

    auto camera_component = std::make_unique<CameraComponent>(viewer_object, *transform_component);
    camera_component->setPerspectiveProjection(70.0f, WindowSystem::get().getAspect());

    auto viewer_movement_component = std::make_unique<ViewerMovementComponent>(viewer_object, InputSystem::get(), *transform_component);

    viewer_object.addComponent(std::move(transform_component));
    viewer_object.addComponent(std::move(camera_component));
    viewer_object.addComponent(std::move(viewer_movement_component));
}

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

        viewer_object.update(frame_info);
        rendered_object.update(frame_info);

        renderer.render(frame_info);
    }
}