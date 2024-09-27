#include "Instance.h"

#include <iostream>
#include <unordered_set>

#include "VulkanDefines.h"
#include "Logs/LogSystem.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void*)
{
    LogSeverity severity;

    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        severity = LogSeverity::ERROR;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        severity = LogSeverity::WARNING;
        break;
    default:
        severity = LogSeverity::LOG;
        break;
    }

    LogSystem::log(severity, Instance::VALIDATION_LAYERS_PREFIX, callback_data->pMessage);

    return VK_FALSE;
}

VkResult createDebugUtilsMessengerExt(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* create_info,
        const VkAllocationCallbacks* allocator,
        VkDebugUtilsMessengerEXT* debug_messenger)
{
    if (auto func = std::bit_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")))
    {
        return func(instance, create_info, allocator, debug_messenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugUtilsMessengerExt(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debug_messenger,
        const VkAllocationCallbacks* allocator)
{
    auto func = std::bit_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        func(instance, debug_messenger, allocator);
    }
}

Instance::Instance()
{
    if (enable_validation_layers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    createInstance();
    setupDebugMessenger();
}

void Instance::createInstance()
{
    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    VkApplicationInfo application_info = fillApplicationInfo();
    instance_create_info.pApplicationInfo = &application_info;

    auto required_extensions = getRequiredExtensions();
    instance_create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    instance_create_info.ppEnabledExtensionNames = required_extensions.data();

    if (enable_validation_layers)
    {
        instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        instance_create_info.ppEnabledLayerNames = validation_layers.data();

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info = populateDebugMessengerCreateInfo();
        instance_create_info.pNext = std::bit_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_create_info);
    }

    VK_CHECK(vkCreateInstance(&instance_create_info, VulkanDefines::NO_CALLBACK, &instance));
    checkIfInstanceHasGlfwRequiredInstanceExtensions();
}

bool Instance::checkValidationLayerSupport()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : validation_layers)
    {
        auto equals = [layer_name] (const VkLayerProperties& layer)
        {
            return strcmp(layer_name, layer.layerName) == 0;
        };

        if (!std::ranges::any_of(available_layers, equals))
        {
            return false;
        }
    }

    return true;
}

VkApplicationInfo Instance::fillApplicationInfo()
{
    VkApplicationInfo application_info{};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Pixelizer";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "No Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_3;

    return application_info;
}

std::vector<const char*> Instance::getRequiredExtensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if (enable_validation_layers)
    {
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    //TIP: comment if debugging with Renderdoc
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    return extensions;
}

VkDebugUtilsMessengerCreateInfoEXT Instance::populateDebugMessengerCreateInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debugCallback;

    return create_info;
}

void Instance::checkIfInstanceHasGlfwRequiredInstanceExtensions()
{
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    LogSystem::log(LogSeverity::LOG, "Available extensions:");
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions)
    {
        LogSystem::log(LogSeverity::LOG, "\t", extension.extensionName);
        available.emplace(extension.extensionName);
    }

    LogSystem::log(LogSeverity::LOG, "Required extensions:");
    auto required_extensions = getRequiredExtensions();
    for (const auto& required: required_extensions)
    {
        LogSystem::log(LogSeverity::LOG, "\t", required);
        if (!available.contains(required))
        {
            LogSystem::log(LogSeverity::FATAL, "Required extensions:");
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

void Instance::setupDebugMessenger()
{
    if constexpr (!validationLayersEnabled())
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = populateDebugMessengerCreateInfo();
    VK_CHECK(createDebugUtilsMessengerExt(instance, &debug_utils_messenger_create_info, VulkanDefines::NO_CALLBACK, &debug_messenger));
}

Instance::~Instance()
{
    if constexpr (validationLayersEnabled())
    {
        destroyDebugUtilsMessengerExt(instance, debug_messenger, VulkanDefines::NO_CALLBACK);
    }
    vkDestroyInstance(instance, VulkanDefines::NO_CALLBACK);
}