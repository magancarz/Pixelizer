#include "PhysicalDevice.h"

#include <iostream>
#include <set>

#include "VulkanSystem.h"
#include "SwapChainSupportDetails.h"
#include "Logs/LogSystem.h"

PhysicalDevice::PhysicalDevice(Instance& instance, Surface& surface)
    : instance{instance}, surface{surface}, used_physical_device{pickPhysicalDevice()},
    queue_family_indices{findQueueFamilies(used_physical_device)} {}

VkPhysicalDevice PhysicalDevice::pickPhysicalDevice()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance.getInstance(), &device_count, nullptr);
    if (device_count == 0)
    {
        LogSystem::log(LogSeverity::FATAL, "Failed to find GPUs with Vulkan support!");
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
    LogSystem::log(LogSeverity::LOG, "Device count: ", device_count);

    std::vector<VkPhysicalDevice> available_physical_devices(device_count);
    vkEnumeratePhysicalDevices(instance.getInstance(), &device_count, available_physical_devices.data());

    VkPhysicalDevice most_suitable_physical_device = findMostSuitablePhysicalDevice(available_physical_devices);
    if (most_suitable_physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(most_suitable_physical_device, &properties);
    LogSystem::log(LogSeverity::LOG, "Physical device: ", properties.deviceName);

    return most_suitable_physical_device;
}

VkPhysicalDevice PhysicalDevice::findMostSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& available_physical_devices)
{
    for (const auto& physical_device : available_physical_devices)
    {
        if (isDeviceSuitable(physical_device))
        {
            return physical_device;
        }
    }

    return VK_NULL_HANDLE;
}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
{
    // #lizard forgives complexity

    return areRequiredExtensionsSupported(device) &&
        areRequiredFeaturesSupported(device) &&
        areRequiredPropertiesFulfilled(device) &&
        isSwapChainSupported(device) &&
        areAllRequiredQueuesPresent(device);
}

bool PhysicalDevice::areRequiredExtensionsSupported(VkPhysicalDevice device)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extension_count,
        available_extensions.data());

    std::set<std::string, std::less<>> required_extensions(device_extensions.begin(), device_extensions.end());

    for (const auto& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

bool PhysicalDevice::isSwapChainSupported(VkPhysicalDevice device)
{
    SwapChainSupportDetails swap_chain_support = querySwapChainSupport(device);
    return !swap_chain_support.formats.empty() && !swap_chain_support.presentModes.empty();
}

bool PhysicalDevice::areRequiredFeaturesSupported(VkPhysicalDevice device)
{
    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device, &supported_features);
    return supported_features.samplerAnisotropy;
}

bool PhysicalDevice::areRequiredPropertiesFulfilled(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties device_properties{};
    vkGetPhysicalDeviceProperties(device, &device_properties);
    return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

bool PhysicalDevice::areAllRequiredQueuesPresent(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice device)
{
    // #lizard forgives complexity

    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    VkQueueFamilyProperties* queue_family_properties{nullptr};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; ++queue_family_index)
    {
        VkQueueFamilyProperties queue_family = queue_families[queue_family_index];

        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphics_family = queue_family_index;
        if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) indices.compute_family = queue_family_index;
        if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) indices.transfer_family = queue_family_index;
        if (queueHasPresentSupport(device, queue_family_index)) indices.present_family = queue_family_index;

        if (indices.isComplete()) break;
    }

    return indices;
}

bool PhysicalDevice::queueHasPresentSupport(VkPhysicalDevice physical_device, uint32_t queue_family_index)
{
    VkBool32 present_support{false};
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface.getSurface(), &present_support);
    return present_support;
}

SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.getSurface(), &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getSurface(), &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getSurface(), &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getSurface(), &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        details.presentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface.getSurface(),
            &present_mode_count,
            details.presentModes.data());
    }
    return details;
}

VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    // #lizard forgives complexity

    for (VkFormat format : candidates)
    {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(used_physical_device, format, &format_properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (format_properties.linearTilingFeatures & features) == features)
        {
            return format;
        }

        if (tiling == VK_IMAGE_TILING_OPTIMAL && (format_properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}
