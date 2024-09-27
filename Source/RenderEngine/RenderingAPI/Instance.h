#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Instance
{
public:
    Instance();
    ~Instance();

    [[nodiscard]] VkInstance getInstance() const { return instance; }
    [[nodiscard]] static constexpr bool validationLayersEnabled() { return enable_validation_layers; }
    [[nodiscard]] std::vector<const char*> getEnabledValidationLayers() const { return validation_layers; }

    static constexpr const char* VALIDATION_LAYERS_PREFIX{"Vulkan validation layers: "};

private:
#ifdef NDEBUG
    static constexpr bool enable_validation_layers{false};
#else
    static constexpr bool enable_validation_layers{true};
#endif

    void createInstance();
    VkApplicationInfo fillApplicationInfo();
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();
    void checkIfInstanceHasGlfwRequiredInstanceExtensions();
    void setupDebugMessenger();

    VkInstance instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT debug_messenger{VK_NULL_HANDLE};

    const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
};
