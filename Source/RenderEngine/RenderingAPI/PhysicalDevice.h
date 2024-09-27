#pragma once

#include "Instance.h"
#include "Surface.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

class PhysicalDevice
{
public:
    PhysicalDevice(Instance& instance, Surface& surface);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return used_physical_device; }
    [[nodiscard]] QueueFamilyIndices getQueueFamilyIndices() const { return queue_family_indices; }
    [[nodiscard]] std::vector<const char*> getDeviceExtensions() const { return device_extensions; }

private:
    Instance& instance;
    Surface& surface;

    VkPhysicalDevice pickPhysicalDevice();
    VkPhysicalDevice findMostSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& available_physical_devices);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool areRequiredExtensionsSupported(VkPhysicalDevice device);
    bool isSwapChainSupported(VkPhysicalDevice device);
    static bool areRequiredFeaturesSupported(VkPhysicalDevice device);
    static bool areRequiredPropertiesFulfilled(VkPhysicalDevice device);
    bool areAllRequiredQueuesPresent(VkPhysicalDevice device);

    VkPhysicalDevice used_physical_device{VK_NULL_HANDLE};
    VkPhysicalDeviceProperties properties{};

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physical_device);
    bool queueHasPresentSupport(VkPhysicalDevice physical_device, uint32_t queue_family_index);

    QueueFamilyIndices queue_family_indices{};

    const std::vector<const char*> device_extensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };
};
