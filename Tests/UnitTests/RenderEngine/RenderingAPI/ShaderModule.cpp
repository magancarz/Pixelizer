#include "gtest/gtest.h"

#include "RenderEngine/RenderingAPI/ShaderModule.h"

#include <TestUtils.h>

TEST(ShaderModuleTests, shouldCreateValidShaderModuleFromShaderCode)
{
    // given
    const std::string real_world_shader_code_file{"SimpleShader"};

    // when
    auto shader_module = std::make_unique<ShaderModule>(VulkanSystem::get().getLogicalDevice(), real_world_shader_code_file, VK_SHADER_STAGE_FRAGMENT_BIT);

    // then
    EXPECT_NE(shader_module->getShaderModule(), VK_NULL_HANDLE);
    TestUtils::failIfVulkanValidationLayersErrorsWerePresent();
}