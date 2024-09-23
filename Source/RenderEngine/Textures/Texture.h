#pragma once

#include <string>
#include <Assets/Texture/TextureData.h>

#include "RenderEngine/RenderingAPI/VulkanSystem.h"
#include "RenderEngine/RenderingAPI/Memory/VulkanMemoryAllocator.h"

class Texture
{
public:
    Texture(
        PhysicalDevice& physical_device,
        Device& logical_device,
        CommandPool& transfer_command_pool,
        VulkanMemoryAllocator& memory_allocator,
        const VkImageCreateInfo& image_create_info,
        const TextureData& texture_info);
    ~Texture();

    [[nodiscard]] std::string getName() const { return texture_info.name; }

    [[nodiscard]] VkImageView getImageView() const { return image_view; }
    [[nodiscard]] VkSampler getSampler() const { return sampler; }
    [[nodiscard]] VkImageLayout getImageLayout() const { return image_layout; }

    [[nodiscard]] VkDescriptorImageInfo descriptorInfo() const;

    [[nodiscard]] uint32_t width() const { return texture_info.width; }
    [[nodiscard]] uint32_t height() const { return texture_info.height; }

private:
    PhysicalDevice& physical_device;
    Device& logical_device;
    CommandPool& transfer_command_pool;
    VkImageCreateInfo image_create_info;
    TextureData texture_info;
    std::unique_ptr<Image> image_buffer{};

    void createImageView();
    void createImageSampler();

    void copyDataToImage(VulkanMemoryAllocator& memory_allocator, const std::vector<unsigned char>& texture_data);
    void transitionImageLayout(VkImageLayout old_layout, VkImageLayout new_layout);
    void generateMipmaps();

    VkImageView image_view{VK_NULL_HANDLE};
    VkSampler sampler{VK_NULL_HANDLE};
    VkImageLayout image_layout{VK_IMAGE_LAYOUT_UNDEFINED};
};
