#pragma once

#include "Instance.h"
#include "Surface.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

class PhysicalDevice
{
public:
    PhysicalDevice(Instance& instance, Surface& surface);

    explicit operator VkPhysicalDevice () const;

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return used_physical_device; }
    [[nodiscard]] QueueFamilyIndices getQueueFamilyIndices() const { return queue_family_indices; }
    [[nodiscard]] std::vector<const char*> getDeviceExtensions() const { return device_extensions; }

private:
    Instance& instance;
    Surface& surface;

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    VkPhysicalDevice used_physical_device{VK_NULL_HANDLE};
    VkPhysicalDeviceProperties properties{};

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physical_device);

    QueueFamilyIndices queue_family_indices{};

    const std::vector<const char*> device_extensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };
};
