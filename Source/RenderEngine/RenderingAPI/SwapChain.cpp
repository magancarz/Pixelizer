#include "SwapChain.h"

#include <array>
#include <iostream>
#include <climits>
#include <stdexcept>

#include "VulkanDefines.h"
#include "Logs/LogSystem.h"

SwapChain::SwapChain(Surface& surface, PhysicalDevice& physical_device, Device& logical_device, VulkanMemoryAllocator& memory_allocator, VkExtent2D window_extent)
    : surface{surface}, physical_device{physical_device}, logical_device{logical_device}, memory_allocator{memory_allocator},
    graphics_queue{logical_device.getGraphicsQueue()}, present_queue{logical_device.getPresentQueue()}, window_extent{window_extent}
{
    initializeSwapChain();
}

void SwapChain::initializeSwapChain()
{
    createSwapChain();
    createImageViews();
    createDepthResources();
    createSyncObjects();
}

SwapChain::SwapChain(Surface& surface, PhysicalDevice& physical_device, Device& logical_device, VulkanMemoryAllocator& memory_allocator, VkExtent2D window_extent, std::shared_ptr<SwapChain> previous)
    : surface{surface}, physical_device{physical_device}, logical_device{logical_device}, memory_allocator{memory_allocator},
    graphics_queue{logical_device.getGraphicsQueue()}, present_queue{logical_device.getPresentQueue()}, window_extent{window_extent}, old_swap_chain{std::move(previous)}
{
    initializeSwapChain();

    old_swap_chain.reset();
}

SwapChain::~SwapChain()
{
    for (auto imageView: swap_chain_image_views)
    {
        vkDestroyImageView(logical_device.handle(), imageView, nullptr);
    }
    swap_chain_image_views.clear();

    if (swap_chain != nullptr)
    {
        vkDestroySwapchainKHR(logical_device.handle(), swap_chain, nullptr);
        swap_chain = nullptr;
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(logical_device.handle(), render_finished_semaphores[i], nullptr);
        vkDestroySemaphore(logical_device.handle(), image_available_semaphores[i], nullptr);
        vkDestroyFence(logical_device.handle(), in_flight_fences[i], nullptr);
    }
}

VkResult SwapChain::acquireNextImage(uint32_t* image_index)
{
    vkWaitForFences(
        logical_device.handle(),
        1,
        &in_flight_fences[current_frame],
        VK_TRUE,
        UINT_MAX);
    vkResetFences(logical_device.handle(), 1, &in_flight_fences[current_frame]);

    VkResult result = vkAcquireNextImageKHR(
        logical_device.handle(),
        swap_chain,
        UINT_MAX,
        image_available_semaphores[current_frame],
        VK_NULL_HANDLE,
        image_index);

    return result;
}

VkResult SwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t buffers_count, uint32_t* image_index)
{
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::array<VkSemaphore, 1> wait_semaphores = {image_available_semaphores[current_frame]};
    std::array<VkPipelineStageFlags, 1> wait_stages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = wait_semaphores.size();
    submit_info.pWaitSemaphores = wait_semaphores.data();
    submit_info.pWaitDstStageMask = wait_stages.data();

    submit_info.commandBufferCount = buffers_count;
    submit_info.pCommandBuffers = buffers;

    std::array<VkSemaphore, 1> signal_semaphores = {render_finished_semaphores[current_frame]};
    submit_info.signalSemaphoreCount = signal_semaphores.size();
    submit_info.pSignalSemaphores = signal_semaphores.data();

    if (graphics_queue.submitCommandBuffer(1u, &submit_info, in_flight_fences[current_frame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = signal_semaphores.size();
    present_info.pWaitSemaphores = signal_semaphores.data();

    std::array<VkSwapchainKHR, 1> swap_chains = {swap_chain};
    present_info.swapchainCount = swap_chains.size();
    present_info.pSwapchains = swap_chains.data();

    present_info.pImageIndices = image_index;

    auto result = logical_device.getPresentQueue().present(present_info);

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

void SwapChain::createSwapChain()
{
    SwapChainSupportDetails swap_chain_support = physical_device.querySwapChainSupport(physical_device.getPhysicalDevice());

    VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(swap_chain_support.formats);
    VkPresentModeKHR present_mode = chooseSwapPresentMode(swap_chain_support.presentModes);
    VkExtent2D extent = chooseSwapExtent(swap_chain_support.capabilities);

    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
    {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface.getSurface();

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = physical_device.getQueueFamilyIndices();
    std::array<uint32_t, 2> queue_family_indices = {indices.graphics_family.value(), indices.present_family.value()};

    if (indices.graphics_family != indices.present_family)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = queue_family_indices.size();
        create_info.pQueueFamilyIndices = queue_family_indices.data();
    } else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;      // Optional
        create_info.pQueueFamilyIndices = nullptr;  // Optional
    }

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    create_info.oldSwapchain = old_swap_chain == nullptr ? VK_NULL_HANDLE : old_swap_chain->swap_chain;

    if (vkCreateSwapchainKHR(logical_device.handle(), &create_info, nullptr, &swap_chain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(logical_device.handle(), swap_chain, &image_count, nullptr);
    swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(logical_device.handle(), swap_chain, &image_count, swap_chain_images.data());

    swap_chain_image_format = surface_format.format;
    swap_chain_extent = extent;
}

void SwapChain::createImageViews()
{
    swap_chain_image_views.resize(swap_chain_images.size());
    for (size_t i = 0; i < swap_chain_images.size(); i++)
    {
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = swap_chain_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swap_chain_image_format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logical_device.handle(), &view_info, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

void SwapChain::createDepthResources()
{
    swap_chain_depth_image_format = findDepthFormat();
    VkExtent2D swap_chain_extent = getSwapChainExtent();

    swap_chain_depth_images.resize(MAX_FRAMES_IN_FLIGHT);
    swap_chain_depth_image_views.resize(MAX_FRAMES_IN_FLIGHT);

    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = swap_chain_extent.width;
    image_info.extent.height = swap_chain_extent.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = swap_chain_depth_image_format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;

    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = swap_chain_depth_image_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    for (int i = 0; i < swap_chain_depth_images.size(); i++)
    {
        swap_chain_depth_images[i] = memory_allocator.createImage(image_info);

        view_info.image = swap_chain_depth_images[i]->getImage();
        if (vkCreateImageView(logical_device.handle(), &view_info, VulkanDefines::NO_CALLBACK, &swap_chain_depth_image_views[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

VkFormat SwapChain::findDepthFormat()
{
    return physical_device.findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void SwapChain::createSyncObjects()
{
    image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    images_in_flight.resize(imageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(logical_device.handle(), &semaphore_info, VulkanDefines::NO_CALLBACK, &image_available_semaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logical_device.handle(), &semaphore_info, VulkanDefines::NO_CALLBACK, &render_finished_semaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logical_device.handle(), &fence_info, VulkanDefines::NO_CALLBACK, &in_flight_fences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
    for (const auto& available_format: available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }

    return available_formats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
    for (const auto& available_present_mode: available_present_modes)
    {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            LogSystem::log(LogSeverity::LOG, "Present mode: Mailbox");
            return available_present_mode;
        }
    }

    LogSystem::log(LogSeverity::LOG, "Present mode: V-Sync");
    return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actual_extent = window_extent;
    actual_extent.width = std::max(
        capabilities.minImageExtent.width,
        std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(
        capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actual_extent.height));

    return actual_extent;
}
