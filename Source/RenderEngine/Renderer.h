#pragma once

#include "FrameInfo.h"
#include "Assets/AssetManager.h"
#include "Editor/Window/Window.h"
#include "RenderEngine/RenderingAPI/SwapChain.h"
#include "RenderEngine/RenderingAPI/CommandBuffer/CommandBuffer.h"
#include "RenderingAPI/Pipeline.h"
#include "RenderingAPI/Descriptors/DescriptorPool.h"
#include "RenderingAPI/Descriptors/DescriptorSetLayout.h"

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

    void createCameraDescriptorSet();

    std::array<std::unique_ptr<Buffer>, SwapChain::MAX_FRAMES_IN_FLIGHT> camera_uniform_buffers{};

    std::unique_ptr<DescriptorPool> descriptor_pool;
    std::unique_ptr<DescriptorSetLayout> camera_descriptor_set_layout;
    std::array<VkDescriptorSet, SwapChain::MAX_FRAMES_IN_FLIGHT> camera_descriptor_set_handles;

    void createSimplePipeline();

    VkPipelineLayout simple_pipeline_layout{VK_NULL_HANDLE};
    std::unique_ptr<Pipeline> simple_pipeline;

    VkCommandBuffer beginFrame();
    void endFrame(VkCommandBuffer graphics_command_buffer);

    void beginRenderPass(VkCommandBuffer command_buffer);
    void endRenderPass(VkCommandBuffer command_buffer);

    bool is_frame_in_progress{false};
    int current_frame_index{0};
    uint32_t current_image_index{0};
};
