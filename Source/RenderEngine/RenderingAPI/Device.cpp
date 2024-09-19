#include "Device.h"

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

Device::~Device()
{
    vkDestroyDevice(device, nullptr);
}

VkDevice Device::createLogicalDevice(const Instance& instance, const PhysicalDevice& physical_device)
{
    QueueFamilyIndices indices = physical_device.getQueueFamilyIndices();

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::unordered_set<uint32_t> unique_queue_families
    {
        indices.graphics_family.value(),
        indices.compute_family.value(),
        indices.present_family.value(),
        indices.transfer_family.value()
    };

    float queue_priority = 1.0f;
    for (uint32_t queue_family: unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    auto device_extensions = physical_device.getDeviceExtensions();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    create_info.pNext = &dynamic_rendering_features;

    auto validation_layers = instance.getEnabledValidationLayers();
    if (instance.validationLayersEnabled())
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }

    VkDevice device{VK_NULL_HANDLE};
    if (vkCreateDevice(physical_device.getPhysicalDevice(), &create_info, VulkanDefines::NO_CALLBACK, &device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device!");
    }

    return device;
}

std::mutex& Device::fetchQueueMutex(uint32_t queue_family_index, uint32_t queue_index)
{
    assert(queue_mutexes.size() > queue_family_index);
    return queue_mutexes[queue_family_index];
}
