#include "Device.h"

#include <iostream>
#include <unordered_set>

#include "VulkanDefines.h"

Device::Device(const Instance& instance, const PhysicalDevice& physical_device)
    : queue_family_indices{physical_device.getQueueFamilyIndices()},
    queue_mutexes(queue_family_indices.uniqueIndices()),
    device{createLogicalDevice(instance, physical_device)},
    graphics_queue{*this, queue_family_indices.graphics_family.value()},
    present_queue{*this, queue_family_indices.present_family.value()},
    compute_queue{*this, queue_family_indices.compute_family.value()},
    transfer_queue{*this, queue_family_indices.transfer_family.value()} {}

VkDevice Device::createLogicalDevice(const Instance& instance, const PhysicalDevice& physical_device)
{
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos = fillQueueCreateInfos();

    VkPhysicalDeviceVulkan12Features vulkan_12_features = fillVulkan12FeaturesInfo();
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = fillDynamicRenderingFeatureInfo();
    dynamic_rendering_features.pNext = &vulkan_12_features;

    VkPhysicalDeviceFeatures device_features = fillPhysicalDeviceFeaturesInfo();

    VkDeviceCreateInfo create_info{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    create_info.pEnabledFeatures = &device_features;

    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    auto device_extensions = physical_device.getDeviceExtensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    create_info.pNext = &dynamic_rendering_features;

    std::vector<const char*> validation_layers = instance.getEnabledValidationLayers();
    fillValidationLayersInfo(instance, create_info, validation_layers);

    VkDevice new_logical_device{VK_NULL_HANDLE};
    VK_CHECK(vkCreateDevice(physical_device.getPhysicalDevice(), &create_info, VulkanDefines::NO_CALLBACK, &new_logical_device));

    return new_logical_device;
}

std::vector<VkDeviceQueueCreateInfo> Device::fillQueueCreateInfos()
{
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::unordered_set<uint32_t> unique_queue_families
    {
        queue_family_indices.graphics_family.value(),
        queue_family_indices.compute_family.value(),
        queue_family_indices.present_family.value(),
        queue_family_indices.transfer_family.value()
    };

    for (uint32_t queue_family: unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.emplace_back(queue_create_info);
    }

    return queue_create_infos;
}

VkPhysicalDeviceVulkan12Features Device::fillVulkan12FeaturesInfo()
{
    VkPhysicalDeviceVulkan12Features vulkan_12_features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    vulkan_12_features.bufferDeviceAddress = VK_TRUE;
    vulkan_12_features.bufferDeviceAddressCaptureReplay = VK_FALSE;
    vulkan_12_features.bufferDeviceAddressMultiDevice = VK_FALSE;

    return vulkan_12_features;
}

VkPhysicalDeviceDynamicRenderingFeaturesKHR Device::fillDynamicRenderingFeatureInfo()
{
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR};
    dynamic_rendering_features.dynamicRendering = VK_TRUE;

    return dynamic_rendering_features;
}

VkPhysicalDeviceFeatures Device::fillPhysicalDeviceFeaturesInfo()
{
    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;

    return device_features;
}

void Device::fillValidationLayersInfo(const Instance& instance, VkDeviceCreateInfo& device_create_info, const std::vector<const char*>& validation_layers)
{
    if (instance.validationLayersEnabled())
    {
        device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        device_create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
        device_create_info.enabledLayerCount = 0;
    }
}

Device::~Device()
{
    vkDestroyDevice(device, VulkanDefines::NO_CALLBACK);
}

std::mutex& Device::fetchQueueMutex(uint32_t queue_family_index)
{
    assert(queue_mutexes.size() > queue_family_index);
    return queue_mutexes[queue_family_index];
}
