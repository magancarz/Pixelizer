#include "gtest/gtest.h"

#include "RenderEngine/RenderingAPI/Descriptors/DescriptorPoolBuilder.h"
#include "TestUtils.h"
#include "Environment.h"

TEST(DescriptorPoolBuilderTests, shouldBuildInvalidDescriptorPoolWhenGivenBindingsAreEmpty)
{
    // given
    auto descriptor_pool_builder = DescriptorPoolBuilder(VulkanSystem::get().getLogicalDevice());

    // when
    auto descriptor_pool = descriptor_pool_builder.build();

    // then
    EXPECT_EQ(descriptor_pool, nullptr);
    TestUtils::failIfVulkanValidationLayersErrorsWerePresent();
}

TEST(DescriptorPoolBuilderTests, shouldBuildValidDescriptorPool)
{
    // given
    auto descriptor_pool_builder = DescriptorPoolBuilder(VulkanSystem::get().getLogicalDevice())
        .setMaxSets(2)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
        .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4)
        .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1)
        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);

    // when
    auto descriptor_pool = descriptor_pool_builder.build();

    // then
    EXPECT_NE(descriptor_pool->descriptorPool(), VK_NULL_HANDLE);
    TestUtils::failIfVulkanValidationLayersErrorsWerePresent();
}