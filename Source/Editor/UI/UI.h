#pragma once

#include <Editor/Window/Window.h>

#include "RenderEngine/RenderingAPI/Descriptors/DescriptorPool.h"
#include "RenderEngine/RenderingAPI/SwapChain.h"
#include "RenderEngine/FrameInfo.h"

class UI
{
public:
    UI(Instance& instance, PhysicalDevice& physical_device, Device& device, const Queue& graphics_queue, Window& window, SwapChain* swap_chain);
    ~UI();

    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;

    void updateUIElements(FrameInfo& frame_info);
    void renderUIElements(VkCommandBuffer command_buffer, uint32_t current_frame_index) const;

    void handleWindowResize(SwapChain* new_swap_chain);

private:
    Instance& instance;
    PhysicalDevice& physical_device;
    Device& device;
    const Queue& graphics_queue;
    Window& window;
    SwapChain* swap_chain{nullptr};

    void initializeImGui();
    static void createImGUIContext();

    void createDescriptorPool();

    std::unique_ptr<DescriptorPool> descriptor_pool;

    void setupRendererBackends();

    static void startNewFrame();

    bool pixelize{false};
};