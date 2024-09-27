#include "Texture.h"

#include <iostream>
#include <utility>

#include "RenderEngine/RenderingAPI/Memory/Buffer.h"
#include "RenderEngine/RenderingAPI/VulkanDefines.h"
#include "RenderEngine/RenderingAPI/Memory/Image.h"
#include "RenderEngine/RenderingAPI/CommandBuffer/SingleTimeCommandBuffer.h"

Texture::Texture(
        VulkanSystem& vulkan_system,
        VulkanMemoryAllocator& memory_allocator,
        TextureInfo texture_info,
        const VkImageCreateInfo& image_create_info)
    : physical_device{vulkan_system.getPhysicalDevice()},
    logical_device{vulkan_system.getLogicalDevice()},
    transfer_command_pool{vulkan_system.getTransferCommandPool()},
    memory_allocator{memory_allocator},
    texture_info{std::move(texture_info)},
    image_create_info{image_create_info},
    image{memory_allocator.createImage(image_create_info)}
{
    createImageView();
    createImageSampler();
}

void Texture::createImageView()
{
    VkImageViewCreateInfo image_view_create_info{};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = image_create_info.format;
    image_view_create_info.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;
    image_view_create_info.subresourceRange.levelCount = image_create_info.mipLevels;
    image_view_create_info.image = image->getImage();

    VK_CHECK(vkCreateImageView(logical_device.handle(), &image_view_create_info, VulkanDefines::NO_CALLBACK, &image_view));
}

void Texture::createImageSampler()
{
    VkSamplerCreateInfo sampler_create_info{};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.compareOp = VK_COMPARE_OP_NEVER;
    sampler_create_info.minLod = 0.0f;
    sampler_create_info.maxLod = static_cast<float>(image_create_info.mipLevels);
    sampler_create_info.maxAnisotropy = 4.0;
    sampler_create_info.anisotropyEnable = VK_TRUE;
    sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    VK_CHECK(vkCreateSampler(logical_device.handle(), &sampler_create_info, VulkanDefines::NO_CALLBACK, &sampler));
}

Texture::~Texture()
{
    vkDestroyImageView(logical_device.handle(), image_view, VulkanDefines::NO_CALLBACK);
    vkDestroySampler(logical_device.handle(), sampler, VulkanDefines::NO_CALLBACK);
}

void Texture::writeTextureData(const std::vector<unsigned char>& data)
{
    assert(image_layout == VK_IMAGE_LAYOUT_UNDEFINED && "Current implementation asserts that texture will be written once right after its creation.");
    assert(data.size() == texture_info.width * texture_info.height * texture_info.number_of_channels && "Data size must match texture size");

    VkImageMemoryBarrier image_memory_barrier = fillBasicImageMemoryBarrierInfo(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    constexpr VkPipelineStageFlags source_stage_mask{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
    constexpr VkPipelineStageFlags destination_stage_mask{VK_PIPELINE_STAGE_TRANSFER_BIT};
    transitionImageLayout(image_memory_barrier, source_stage_mask, destination_stage_mask);

    copyDataToImage(data);
    generateMipmaps();
}

VkImageMemoryBarrier Texture::fillBasicImageMemoryBarrierInfo(VkImageLayout old_layout, VkImageLayout new_layout)
{
    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.oldLayout = old_layout;
    image_memory_barrier.newLayout = new_layout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image->getImage();
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = image_create_info.mipLevels;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;

    return image_memory_barrier;
}

void Texture::transitionImageLayout(
        const VkImageMemoryBarrier& image_memory_barrier,
        VkPipelineStageFlags source_stage_mask,
        VkPipelineStageFlags destination_stage_mask)
{
    SingleTimeCommandBuffer single_time_command_buffer = transfer_command_pool.createSingleTimeCommandBuffer();
    VkCommandBuffer command_buffer = single_time_command_buffer.beginRecording();

    vkCmdPipelineBarrier(
        command_buffer,
        source_stage_mask,
        destination_stage_mask,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &image_memory_barrier);

    const Fence& fence = single_time_command_buffer.endRecordingAndSubmit();
    image_layout = image_memory_barrier.newLayout;
    fence.wait();
}

void Texture::copyDataToImage(const std::vector<unsigned char>& data)
{
    auto staging_buffer = memory_allocator.createStagingBuffer(texture_info.number_of_channels, texture_info.width * texture_info.height, data.data());

    SingleTimeCommandBuffer single_time_command_buffer = transfer_command_pool.createSingleTimeCommandBuffer();
    VkCommandBuffer command_buffer = single_time_command_buffer.beginRecording();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {texture_info.width, texture_info.height, 1};

    vkCmdCopyBufferToImage(
        command_buffer,
        staging_buffer->getBuffer(),
        image->getImage(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    const Fence& fence = single_time_command_buffer.endRecordingAndSubmit();
    fence.wait();
}

void Texture::generateMipmaps()
{
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(physical_device.getPhysicalDevice(), image_create_info.format, &format_properties);

    if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    SingleTimeCommandBuffer single_time_command_buffer = transfer_command_pool.createSingleTimeCommandBuffer();
    VkCommandBuffer command_buffer = single_time_command_buffer.beginRecording();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image->getImage();
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    auto mip_width = static_cast<int32_t>(texture_info.width);
    auto mip_height = static_cast<int32_t>(texture_info.height);

    for (uint32_t i = 1; i < image_create_info.mipLevels; ++i)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mip_width, mip_height, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(
            command_buffer,
            image->getImage(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image->getImage(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

        if (mip_width > 1) mip_width /= 2;
        if (mip_height > 1) mip_height /= 2;
    }

    barrier.subresourceRange.baseMipLevel = image_create_info.mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    const Fence& fence = single_time_command_buffer.endRecordingAndSubmit();
    fence.wait();

    image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

VkDescriptorImageInfo Texture::descriptorInfo() const
{
    VkDescriptorImageInfo descriptor_info{};
    descriptor_info.sampler = sampler;
    descriptor_info.imageView = image_view;
    descriptor_info.imageLayout = image_layout;

    return descriptor_info;
}