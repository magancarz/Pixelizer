#pragma once

#include <memory>
#include <vector>

#include "VulkanSystem.h"
#include "Memory/Image.h"
#include "Memory/VulkanMemoryAllocator.h"

class SwapChain
{
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT{3};

    SwapChain(Surface& surface, PhysicalDevice& physical_device, Device& logical_device, VulkanMemoryAllocator& memory_allocator, VkExtent2D window_extent);
    SwapChain(Surface& surface, PhysicalDevice& physical_device, Device& logical_device, VulkanMemoryAllocator& memory_allocator, VkExtent2D window_extent, std::shared_ptr<SwapChain> previous);
    ~SwapChain();

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

    [[nodiscard]] uint32_t getCurrentFrameIndex() const { return current_frame; }
    [[nodiscard]] VkImageView getImageView(uint32_t index) const { return swap_chain_image_views[index]; }
    [[nodiscard]] VkImageView getDepthImageView(uint32_t index) const { return swap_chain_depth_image_views[index]; }
    [[nodiscard]] VkImage getImage(uint32_t index) const { return swap_chain_images[index]; }
    [[nodiscard]] VkImage getDepthImage(uint32_t index) const { return swap_chain_depth_images[index]->getImage(); }
    [[nodiscard]] size_t imageCount() const { return swap_chain_images.size(); }
    [[nodiscard]] VkFormat getSwapChainImageFormat() const { return swap_chain_image_format; }
    [[nodiscard]] VkFormat getSwapChainDepthImageFormat() const { return swap_chain_depth_image_format; }
    [[nodiscard]] VkExtent2D getSwapChainExtent() const { return swap_chain_extent; }
    [[nodiscard]] uint32_t width() const { return swap_chain_extent.width; }
    [[nodiscard]] uint32_t height() const { return swap_chain_extent.height; }

    [[nodiscard]] float extentAspectRatio() const { return static_cast<float>(swap_chain_extent.width) / static_cast<float>(swap_chain_extent.height); }

    VkResult acquireNextImage(uint32_t* image_index);
    VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t buffers_count, uint32_t* image_index);

    [[nodiscard]] bool compareSwapChainFormats(const SwapChain& swap_chain) const
    {
        return swap_chain.swap_chain_image_format == swap_chain_image_format;
    }

private:
    Surface& surface;
    PhysicalDevice& physical_device;
    Device& logical_device;
    VulkanMemoryAllocator& memory_allocator;
    const Queue& graphics_queue;
    const Queue& present_queue;
    VkExtent2D window_extent;

    void initializeSwapChain();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    VkFormat findDepthFormat();
    void createSyncObjects();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkFormat swap_chain_image_format;
    VkFormat swap_chain_depth_image_format;
    VkExtent2D swap_chain_extent;

    std::vector<VkImage> swap_chain_images;
    std::vector<VkImageView> swap_chain_image_views;
    std::vector<std::unique_ptr<Image>> swap_chain_depth_images;
    std::vector<VkImageView> swap_chain_depth_image_views;

    VkSwapchainKHR swap_chain{VK_NULL_HANDLE};
    std::shared_ptr<SwapChain> old_swap_chain;

    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkSemaphore> render_finished_semaphores;
    std::vector<VkFence> in_flight_fences;
    std::vector<VkFence> images_in_flight;
    size_t current_frame{0};
};
