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

    std::mutex& fetchQueueMutex(uint32_t queue_family_index, uint32_t queue_index);

private:
    QueueFamilyIndices queue_family_indices;
    std::vector<std::mutex> queue_mutexes;

    VkDevice createLogicalDevice(const Instance& instance, const PhysicalDevice& physical_device);

    VkDevice device;

    Queue graphics_queue;
    Queue present_queue;
    Queue compute_queue;
    Queue transfer_queue;
};
