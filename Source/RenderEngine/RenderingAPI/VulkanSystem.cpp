#include "VulkanSystem.h"

VulkanSystem& VulkanSystem::initialize()
{
    vulkan_system = std::unique_ptr<VulkanSystem>(new VulkanSystem());

    pvkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(
        vkGetDeviceProcAddr(vulkan_system->getLogicalDevice().handle(), "vkCmdBeginRenderingKHR"));
    pvkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(
        vkGetDeviceProcAddr(vulkan_system->getLogicalDevice().handle(), "vkCmdEndRenderingKHR"));

    return *vulkan_system;
}
