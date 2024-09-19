#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Instance
{
public:
    Instance();
    ~Instance();

    VkInstance getInstance() { return instance; }
    [[nodiscard]] bool validationLayersEnabled() const { return enable_validation_layers; }
    [[nodiscard]] std::vector<const char*> getEnabledValidationLayers() const { return validation_layers; }

    static constexpr const char* VALIDATION_LAYERS_PREFIX{"Vulkan validation layers: "};

private:
#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

    void createInstance();
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
    void checkIfInstanceHasGlfwRequiredInstanceExtensions();
    void setupDebugMessenger();

    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
};
