#include "gtest/gtest.h"

#include "RenderEngine/RenderingAPI/Descriptors/DescriptorSetLayoutBuilder.h"

#include <TestUtils.h>

TEST(DescriptorSetLayoutBuilderTests, shouldBuildInvalidDescriptorSetLayoutWhenGivenBindingsAreEmpty)
{
    // given
    auto descriptor_set_layout_builder = DescriptorSetLayoutBuilder(VulkanSystem::get().getLogicalDevice());

    // when
    auto descriptor_set_layout = descriptor_set_layout_builder.build();

    // then
    EXPECT_EQ(descriptor_set_layout, nullptr);
    TestUtils::failIfVulkanValidationLayersErrorsWerePresent();
}

TEST(DescriptorSetLayoutBuilderTests, shouldBuildValidDescriptorSetLayout)
{
    // given
    auto descriptor_set_layout_builder = DescriptorSetLayoutBuilder(VulkanSystem::get().getLogicalDevice())
        .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
        .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
        .addBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
        .addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
        .addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
        .addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT);

    // when
    auto descriptor_set_layout = descriptor_set_layout_builder.build();

    // then
    EXPECT_NE(descriptor_set_layout->getDescriptorSetLayout(), VK_NULL_HANDLE);
    TestUtils::failIfVulkanValidationLayersErrorsWerePresent();
}