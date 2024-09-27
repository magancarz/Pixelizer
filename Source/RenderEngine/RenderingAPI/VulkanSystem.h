#pragma once

#include <memory>

#include "Instance.h"
#include "Surface.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "CommandBuffer/CommandPool.h"

inline PFN_vkCmdBeginRenderingKHR pvkCmdBeginRenderingKHR;
inline PFN_vkCmdEndRenderingKHR pvkCmdEndRenderingKHR;

class VulkanMemoryAllocator;

class VulkanSystem
{
public:
    static VulkanSystem& initialize();
    [[nodiscard]] static VulkanSystem& get() { return *vulkan_system; }

    [[nodiscard]] Instance& getInstance() { return instance; }
    [[nodiscard]] Surface& getSurface() { return surface; }
    [[nodiscard]] PhysicalDevice& getPhysicalDevice() { return used_physical_device; }
    [[nodiscard]] Device& getLogicalDevice() { return device; }

    [[nodiscard]] const Queue& getGraphicsQueue() const { return device.getGraphicsQueue(); }
    [[nodiscard]] const Queue& getPresentQueue() const { return device.getPresentQueue(); }
    [[nodiscard]] const Queue& getComputeQueue() const { return device.getComputeQueue(); }
    [[nodiscard]] const Queue& getTransferQueue() const { return device.getTransferQueue(); }

    [[nodiscard]] const CommandPool& getGraphicsCommandPool() const { return graphics_command_pool; }
    [[nodiscard]] const CommandPool& getComputeCommandPool() const { return compute_command_pool; }
    [[nodiscard]] const CommandPool& getTransferCommandPool() const { return transfer_command_pool; }

private:
    VulkanSystem() = default;

    Instance instance;
    Surface surface{instance};
    PhysicalDevice used_physical_device{instance, surface};
    Device device{instance, used_physical_device};

    CommandPool graphics_command_pool{device, device.getGraphicsQueue()};
    CommandPool compute_command_pool{device, device.getComputeQueue()};
    CommandPool transfer_command_pool{device, device.getTransferQueue()};

    inline static std::unique_ptr<VulkanSystem> vulkan_system;
};
