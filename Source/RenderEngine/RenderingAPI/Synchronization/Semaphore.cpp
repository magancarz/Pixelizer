#include "Semaphore.h"

#include "Logs/LogSystem.h"
#include "RenderEngine/RenderingAPI/VulkanDefines.h"

Semaphore::Semaphore(VkDevice device, VkSemaphoreCreateFlags flags)
    : device{device}, semaphore{createSemaphore(flags)} {}

VkSemaphore Semaphore::createSemaphore(VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = flags;

    VkSemaphore semaphore{VK_NULL_HANDLE};

    if (vkCreateSemaphore(device, &semaphore_info, VulkanDefines::NO_CALLBACK, &semaphore) != VK_SUCCESS)
    {
        const char* message = "Failed to create semaphore!";
        LogSystem::log(LogSeverity::FATAL, message);
        throw std::runtime_error(message);
    }

    return semaphore;
}

Semaphore::~Semaphore()
{
    vkDestroySemaphore(device, semaphore, VulkanDefines::NO_CALLBACK);
}
