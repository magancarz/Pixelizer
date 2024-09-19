#include "PhysicalDevice.h"

#include <iostream>
#include <set>

#include "VulkanSystem.h"
#include "SwapChainSupportDetails.h"
#include "Logs/LogSystem.h"

PhysicalDevice::PhysicalDevice(Instance& instance, Surface& surface)
    : instance{instance}, surface{surface}
{
    pickPhysicalDevice();
}

PhysicalDevice::operator VkPhysicalDevice () const
{
    return used_physical_device;
}

void PhysicalDevice::pickPhysicalDevice()
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

    for (const auto& physical_device : available_physical_devices)
    {
        if (isDeviceSuitable(physical_device))
        {
            used_physical_device = physical_device;
            break;
        }
    }

    if (used_physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(used_physical_device, &properties);
    queue_family_indices = findQueueFamilies(used_physical_device);
    LogSystem::log(LogSeverity::LOG, "Physical device: ", properties.deviceName);
}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensions_supported = checkDeviceExtensionSupport(device);

    bool swap_chain_adequate;
    if (extensions_supported)
    {
        SwapChainSupportDetails swap_chain_support = querySwapChainSupport(device);
        swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device, &supported_features);

    VkPhysicalDeviceProperties device_properties{};
    vkGetPhysicalDeviceProperties(device, &device_properties);

    return indices.isComplete() &&
        extensions_supported &&
        swap_chain_adequate &&
        supported_features.samplerAnisotropy &&
        device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extension_count,
        available_extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

    for (const auto& extension: available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto& queue_family: queue_families)
    {
        if (queue_family.queueCount == 0)
        {
            continue;
        }

        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
        }

        if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            indices.compute_family = i;
        }

        if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            indices.transfer_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.getSurface(), &present_support);
        if (present_support)
        {
            indices.present_family = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        ++i;
    }

    return indices;
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

uint32_t PhysicalDevice::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(used_physical_device, &memory_properties);
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}