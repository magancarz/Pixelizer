#include "Renderer.h"

#include <thread>

#include "imgui.h"
#include "Logs/LogSystem.h"

#include "RenderEngine/RenderingAPI/Descriptors/DescriptorWriter.h"
#include "RenderingAPI/VulkanDefines.h"
#include "RenderingAPI/CommandBuffer/CommandBuffer.h"

Renderer::Renderer(
        Window& window,
        Surface& surface,
        PhysicalDevice& physical_device,
        Device& logical_device,
        const CommandPool& graphics_command_pool,
        VulkanMemoryAllocator& memory_allocator,
        AssetManager& asset_manager)
    : window{window}, surface{surface}, physical_device{physical_device}, device{logical_device},
    graphics_command_pool{graphics_command_pool}, memory_allocator{memory_allocator}, asset_manager{asset_manager}
{
    recreateSwapChain();
    createCommandBuffers();
}

void Renderer::recreateSwapChain()
{
    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = window.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(device.handle());

    if (swap_chain == nullptr)
    {
        swap_chain = std::make_unique<SwapChain>(surface, physical_device, device, extent);
    }
    else
    {
        std::shared_ptr<SwapChain> old_swap_chain = std::move(swap_chain);
        swap_chain = std::make_unique<SwapChain>(surface, physical_device, device, extent, old_swap_chain);

        if (!old_swap_chain->compareSwapChainFormats(*swap_chain))
        {
            throw std::runtime_error("Swap chain image or depth format has changed!");
        }
    }
}

void Renderer::createCommandBuffers()
{
    command_buffers = graphics_command_pool.createCommandBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
}

Renderer::~Renderer()
{
    freeCommandBuffers();
}

void Renderer::freeCommandBuffers()
{
    graphics_command_pool.freeCommandBuffers(command_buffers.size(), command_buffers.data());
    command_buffers.clear();
}

void Renderer::render(FrameInfo& frame_info)
{
    if (auto command_buffer = beginFrame())
    {
        frame_info.graphics_command_buffer = command_buffer;
        frame_info.window_size = swap_chain->getSwapChainExtent();
        frame_info.frame_index = swap_chain->getCurrentFrameIndex();

        beginRenderPass(command_buffer);

        endRenderPass(command_buffer);

        endFrame(command_buffer);
    }
}

VkCommandBuffer Renderer::beginFrame()
{
    assert(!is_frame_in_progress && "Can't call beginFrame while already in progress!");

    auto result = swap_chain->acquireNextImage(&current_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    is_frame_in_progress = true;

    const auto& command_buffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer.handle(), &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    return command_buffer.handle();
}

void Renderer::endFrame(VkCommandBuffer graphics_command_buffer)
{
    assert(is_frame_in_progress && "Can't call endFrame while frame is not in progress");

    if (vkEndCommandBuffer(graphics_command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer!");
    }

    auto result = swap_chain->submitCommandBuffers(&graphics_command_buffer, 1, &current_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
    {
        window.resetWindowResizedFlag();
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    is_frame_in_progress = false;
    current_frame_index = (current_frame_index + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginRenderPass(VkCommandBuffer command_buffer)
{
    assert(is_frame_in_progress && "Can't call beginSwapChainRenderPass if frame is not in progress!");
    assert(command_buffer == getCurrentCommandBuffer().handle() &&
        "Can't begin updateElements pass on command buffer from a different frame!");

    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.image = swap_chain->getImage(current_image_index);
    image_memory_barrier.subresourceRange =
        {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &image_memory_barrier
    );

    VkClearValue clear_value{};
    clear_value.color = VkClearColorValue{.float32 = {0.1f, 0.1f, 0.1f, 1.0f}};

    VkRenderingAttachmentInfoKHR color_attachment_info{};
    color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    color_attachment_info.imageView = swap_chain->getImageView(current_image_index);
    color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_info.clearValue = clear_value;

    VkRect2D render_area{};
    render_area.offset = {0, 0};
    render_area.extent = swap_chain->getSwapChainExtent();

    VkRenderingInfoKHR render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    render_info.renderArea = render_area;
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment_info;

    pvkCmdBeginRenderingKHR(command_buffer, &render_info);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap_chain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swap_chain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{.offset = {0, 0}, .extent = swap_chain->getSwapChainExtent()};

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void Renderer::endRenderPass(VkCommandBuffer command_buffer)
{
    assert(is_frame_in_progress && "Can't call endSwapChainRenderPass if frame is not in progress!");
    assert(command_buffer == getCurrentCommandBuffer().handle() && "Can't end updateElements pass on command buffer from a different frame!");

    pvkCmdEndRenderingKHR(command_buffer);

    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    image_memory_barrier.image = swap_chain->getImage(current_image_index);
    image_memory_barrier.subresourceRange = VkImageSubresourceRange
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &image_memory_barrier
    );
}
