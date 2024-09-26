#pragma once

#include <gtest/gtest.h>

#include "TestLogger.h"
#include "RenderEngine/RenderingAPI/Memory/VulkanMemoryAllocator.h"
#include "RenderEngine/RenderingAPI/VulkanSystem.h"

class TestsEnvironment : public ::testing::Environment
{
public:
    void SetUp() override;
    void TearDown() override;

    static void initializeTestsLogger();

    [[nodiscard]] static VulkanMemoryAllocator& vulkanMemoryAllocator() { return *memory_allocator; }
    [[nodiscard]] static TestLogger& testLogger() { return *test_logger; }

private:
    static void failAllTestsIfThereWereAnyVulkanValidationLayersErrorsDuringSetup() ;

    inline static std::unique_ptr<VulkanMemoryAllocator> memory_allocator;
    inline static TestLogger* test_logger;
};