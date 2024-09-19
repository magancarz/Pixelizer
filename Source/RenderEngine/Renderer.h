#pragma once

#include "FrameInfo.h"
#include "Assets/AssetManager.h"
#include "Editor/Window/Window.h"
#include "RenderEngine/RenderingAPI/SwapChain.h"
#include "RenderEngine/RenderingAPI/Descriptors/DescriptorPool.h"
#include "RenderEngine/RenderingAPI/CommandBuffer/CommandBuffer.h"

class Renderer
{
public:
    Renderer(
        Window& window,
        Surface& surface,
        PhysicalDevice& physical_device,
        Device& logical_device,
        const CommandPool& graphics_command_pool,
        VulkanMemoryAllocator& memory_allocator,
        AssetManager& asset_manager);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void render(FrameInfo& frame_info);

private:
    Window& window;
    Surface& surface;
    PhysicalDevice& physical_device;
    Device& device;
    const CommandPool& graphics_command_pool;
    VulkanMemoryAllocator& memory_allocator;
    AssetManager& asset_manager;

    [[nodiscard]] const CommandBuffer& getCurrentCommandBuffer() const
    {
        assert(is_frame_in_progress && "Cannot get command buffer when frame not in progress");
        return command_buffers[current_frame_index];
    }

    [[nodiscard]] int getFrameIndex() const
    {
        assert(is_frame_in_progress && "Cannot get frame index when frame not in progress!");
        return current_frame_index;
    }

    void recreateSwapChain();

    std::unique_ptr<SwapChain> swap_chain;

    void createCommandBuffers();
    void freeCommandBuffers();

    std::vector<CommandBuffer> command_buffers;

    VkCommandBuffer beginFrame();
    void endFrame(VkCommandBuffer graphics_command_buffer);

    void beginRenderPass(VkCommandBuffer command_buffer);
    void endRenderPass(VkCommandBuffer command_buffer);

    bool is_frame_in_progress{false};
    int current_frame_index{0};
    uint32_t current_image_index{0};

    void handleWindowResize();
};
