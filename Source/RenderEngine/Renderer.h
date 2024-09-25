#pragma once

#include "FrameInfo.h"
#include "Assets/AssetManager.h"
#include "Editor/UI/UI.h"
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
        Instance& instance,
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
    Instance& instance;
    Surface& surface;
    PhysicalDevice& physical_device;
    Device& device;
    const CommandPool& graphics_command_pool;
    VulkanMemoryAllocator& memory_allocator;
    AssetManager& asset_manager;

    std::unique_ptr<SwapChain> recreateSwapChain();

    std::unique_ptr<SwapChain> swap_chain;

    UI ui{instance, physical_device, device, device.getGraphicsQueue(), window, swap_chain.get()};

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

    void createCommandBuffers();
    void freeCommandBuffers();

    std::vector<CommandBuffer> command_buffers;

    void createCameraDescriptorSet();

    std::array<std::unique_ptr<Buffer>, SwapChain::MAX_FRAMES_IN_FLIGHT> camera_uniform_buffers{};

    std::unique_ptr<DescriptorPool> descriptor_pool;
    std::unique_ptr<DescriptorSetLayout> camera_descriptor_set_layout;
    std::array<VkDescriptorSet, SwapChain::MAX_FRAMES_IN_FLIGHT> camera_descriptor_set_handles;

    void createRenderedToTextures();
    void createSimplePipeline();

    std::vector<std::unique_ptr<Texture>> rendered_to_textures;

    VkPipelineLayout simple_pipeline_layout{VK_NULL_HANDLE};
    std::unique_ptr<Pipeline> simple_pipeline;

    void createPostProcessingPipeline();

    std::unique_ptr<DescriptorSetLayout> post_processed_image_descriptor_set_layout;
    std::array<VkDescriptorSet, SwapChain::MAX_FRAMES_IN_FLIGHT> post_processed_image_descriptor_set_handles;
    VkPipelineLayout post_processing_pipeline_layout{VK_NULL_HANDLE};
    std::unique_ptr<Pipeline> post_processing_pipeline;

    VkCommandBuffer beginFrame();
    void endFrame(VkCommandBuffer command_buffer);

    void renderModel(FrameInfo& frame_info);
    void beginRenderingModelRenderPass(VkCommandBuffer command_buffer);
    void endRenderingModelRenderPass(VkCommandBuffer command_buffer);

    void applyPostProcessing(FrameInfo& frame_info);
    void beginPostProcessingRenderPass(VkCommandBuffer command_buffer);
    void endPostProcessingRenderPass(VkCommandBuffer command_buffer);

    bool is_frame_in_progress{false};
    int current_frame_index{0};
    uint32_t current_image_index{0};
};
