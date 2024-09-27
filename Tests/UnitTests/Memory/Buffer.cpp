#include "gtest/gtest.h"

#include <Environment.h>
#include <TestUtils.h>

TEST(BufferTests, shouldMapBufferMemory)
{
    // given
    BufferInfo buffer_info{};
    buffer_info.instance_size = sizeof(uint8_t);
    buffer_info.instance_count = 22;
    buffer_info.usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_info.required_memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer_info.allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    auto buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(buffer_info);

    // when
    buffer->map();

    // then
    EXPECT_TRUE(buffer->getMappedMemory() != nullptr);
}

TEST(BufferTests, shouldUnmapBufferMemory)
{
    // given
    BufferInfo buffer_info{};
    buffer_info.instance_size = sizeof(uint8_t);
    buffer_info.instance_count = 22;
    buffer_info.usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_info.required_memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer_info.allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    auto buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(buffer_info);
    buffer->map();

    ASSERT_TRUE(buffer->getMappedMemory() != nullptr);

    // when
    buffer->unmap();

    // then
    EXPECT_TRUE(buffer->getMappedMemory() == nullptr);
}

TEST(BufferTests, shouldWriteToStagingBuffer)
{
    // given
    std::vector<uint8_t> data_to_write{2, 1, 3, 7};
    auto staging_buffer = TestsEnvironment::vulkanMemoryAllocator().createStagingBuffer(
        sizeof(uint8_t), static_cast<uint32_t>(data_to_write.size()));

    // when
    staging_buffer->writeToBuffer(data_to_write.data());

    // then
    TestUtils::expectBufferHasEqualData(*staging_buffer, data_to_write);
}

TEST(BufferTests, shouldCopyFromStagingBuffer)
{
    // given
    std::vector<uint8_t> data{2, 1, 3, 7};
    auto staging_buffer = TestsEnvironment::vulkanMemoryAllocator().createStagingBuffer(
        sizeof(uint8_t), static_cast<uint32_t>(data.size()), data.data());

    BufferInfo buffer_info{};
    buffer_info.instance_size = sizeof(uint8_t);
    buffer_info.instance_count = static_cast<uint32_t>(data.size());
    buffer_info.usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    auto dest_buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(buffer_info);

    // when
    dest_buffer->copyFrom(*staging_buffer);

    // then
    TestUtils::expectBufferHasEqualData(*dest_buffer, data);
}

TEST(BufferTests, shouldReturnValidBufferHandle)
{
    // given
    BufferInfo buffer_info{};
    buffer_info.instance_size = sizeof(uint8_t);
    buffer_info.instance_count = 22;
    buffer_info.usage_flags =
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    buffer_info.required_memory_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    auto buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(buffer_info);

    // when
    VkBuffer const buffer_handle = buffer->getBuffer();

    // then
    EXPECT_TRUE(buffer_handle != VK_NULL_HANDLE);
}

TEST(BufferTests, shouldReturnCorrectBufferSize)
{
    // given
    BufferInfo buffer_info{};
    buffer_info.instance_size = sizeof(uint8_t);
    buffer_info.instance_count = 22;
    buffer_info.usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    auto buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(buffer_info);

    // when
    uint32_t buffer_size  = buffer->getSize();

    // then
    EXPECT_EQ(buffer_size, buffer_info.instance_size * buffer_info.instance_count);
}

TEST(BufferTests, shouldReturnValidDeviceAddress)
{
    // given
    BufferInfo buffer_info{};
    buffer_info.instance_size = sizeof(uint8_t);
    buffer_info.instance_count = 22;
    buffer_info.usage_flags =
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    buffer_info.required_memory_flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    auto buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(buffer_info);

    // when
    VkDeviceAddress device_address = buffer->getBufferDeviceAddress();

    // then
    EXPECT_TRUE(device_address != 0ULL);
}

TEST(BufferTests, shouldReturnValidDescriptorInfo)
{
    // given
    BufferInfo buffer_info{};
    buffer_info.instance_size = sizeof(uint8_t);
    buffer_info.instance_count = 22;
    buffer_info.usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    auto buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(buffer_info);

    // when
    VkDescriptorBufferInfo descriptor_buffer_info  = buffer->descriptorInfo();

    // then
    EXPECT_NE(descriptor_buffer_info.buffer, VK_NULL_HANDLE);
    EXPECT_EQ(descriptor_buffer_info.offset, 0);
    EXPECT_EQ(descriptor_buffer_info.range, VK_WHOLE_SIZE);
}