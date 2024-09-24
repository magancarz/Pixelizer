#pragma once

#include <string>

#include "Assets/Texture/TextureInfo.h"
#include "RenderEngine/RenderingAPI/VulkanSystem.h"
#include "RenderEngine/RenderingAPI/Memory/VulkanMemoryAllocator.h"

class Texture
{
public:
    Texture(
        PhysicalDevice& physical_device,
        Device& logical_device,
        const CommandPool& transfer_command_pool,
        VulkanMemoryAllocator& memory_allocator,
        const TextureInfo& texture_info);
    ~Texture();

    void writeTextureData(const std::vector<unsigned char>& data);

    [[nodiscard]] std::string getName() const { return texture_info.name; }

    [[nodiscard]] VkSampler getSampler() const { return sampler; }
    [[nodiscard]] VkImage getImage() const { return image->getImage(); }
    [[nodiscard]] VkImageView getImageView() const { return image_view; }
    [[nodiscard]] VkImageLayout getImageLayout() const { return image_layout; }

    [[nodiscard]] VkDescriptorImageInfo descriptorInfo() const;

    [[nodiscard]] uint32_t width() const { return texture_info.width; }
    [[nodiscard]] uint32_t height() const { return texture_info.height; }

private:
    PhysicalDevice& physical_device;
    Device& logical_device;
    const CommandPool& transfer_command_pool;
    VulkanMemoryAllocator& memory_allocator;
    TextureInfo texture_info;
    std::unique_ptr<Image> image{};

    void createImage();
    void createImageView();
    void createImageSampler();

    void transitionImageLayout(VkImageLayout old_layout, VkImageLayout new_layout);
    void generateMipmaps();

    VkImageView image_view{VK_NULL_HANDLE};
    VkSampler sampler{VK_NULL_HANDLE};

    VkImageLayout image_layout{VK_IMAGE_LAYOUT_UNDEFINED};
};