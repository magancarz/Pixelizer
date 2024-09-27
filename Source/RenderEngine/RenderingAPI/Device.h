#pragma once

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Queue/Queue.h"

class Device
{
public:
    Device(const Instance& instance, const PhysicalDevice& physical_device);
    ~Device();

    [[nodiscard]] VkDevice handle() const { return device; }
    [[nodiscard]] const Queue& getGraphicsQueue() const { return graphics_queue; }
    [[nodiscard]] const Queue& getComputeQueue() const { return compute_queue; }
    [[nodiscard]] const Queue& getPresentQueue() const { return present_queue; }
    [[nodiscard]] const Queue& getTransferQueue() const { return transfer_queue; }

    std::mutex& fetchQueueMutex(uint32_t queue_family_index);

private:
    QueueFamilyIndices queue_family_indices;
    std::vector<std::mutex> queue_mutexes;
    float queue_priority{1.0f};

    VkDevice createLogicalDevice(const Instance& instance, const PhysicalDevice& physical_device);
    std::vector<VkDeviceQueueCreateInfo> fillQueueCreateInfos();
    static VkPhysicalDeviceVulkan12Features fillVulkan12FeaturesInfo();
    static VkPhysicalDeviceDynamicRenderingFeaturesKHR fillDynamicRenderingFeatureInfo();
    static VkPhysicalDeviceFeatures fillPhysicalDeviceFeaturesInfo();
    static void fillValidationLayersInfo(const Instance& instance, VkDeviceCreateInfo& device_create_info, const std::vector<const char*>& validation_layers);

    VkDevice device;

    Queue graphics_queue;
    Queue present_queue;
    Queue compute_queue;
    Queue transfer_queue;
};
