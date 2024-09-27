#include "Environment.h"

#include "Editor/Window/GLFWWindow.h"
#include "Editor/Window/WindowSystem.h"
#include "Logs/LogSystem.h"
#include "RenderEngine/RenderingAPI/Memory/VulkanMemoryAllocator.h"
#include "TestLogger.h"

void TestsEnvironment::SetUp()
{
    initializeTestsLogger();

    WindowSystem::initialize(std::make_unique<GLFWWindow>());
    VulkanSystem& vulkan_system = VulkanSystem::initialize();
    memory_allocator = std::make_unique<VulkanMemoryAllocator>(
        vulkan_system.getInstance(),
        vulkan_system.getPhysicalDevice(),
        vulkan_system.getLogicalDevice(),
        vulkan_system.getTransferCommandPool());

    failAllTestsIfThereWereAnyVulkanValidationLayersErrorsDuringSetup();
}

void TestsEnvironment::initializeTestsLogger()
{
    auto new_test_logger = std::make_unique<TestLogger>();
    test_logger = new_test_logger.get();
    LogSystem::initialize(std::move(new_test_logger));
}

void TestsEnvironment::failAllTestsIfThereWereAnyVulkanValidationLayersErrorsDuringSetup()
{
    if (test_logger->anyVulkanValidationLayersErrors())
    {
        GTEST_FATAL_FAILURE_("Vulkan validation layer messages occurred during Vulkan setup!");
    }
}

void TestsEnvironment::TearDown()
{
    memory_allocator.reset();
    WindowSystem::initialize(nullptr);
    LogSystem::initialize(nullptr);
}