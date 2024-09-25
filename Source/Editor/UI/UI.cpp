#include "UI.h"

#define IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

#include "RenderEngine/RenderingAPI/SwapChain.h"
#include "RenderEngine/RenderingAPI/VulkanDefines.h"
#include "RenderEngine/FrameInfo.h"
#include "Editor/Window/GLFWWindow.h"
#include "Editor/Window/WindowSystem.h"
#include "glm/ext/scalar_constants.hpp"
#include "Logs/LogSystem.h"
#include "RenderEngine/RenderingAPI/Descriptors/DescriptorPoolBuilder.h"

UI::UI(Instance& instance, PhysicalDevice& physical_device, Device& device, const Queue& graphics_queue, Window& window, SwapChain* swap_chain)
    : instance{instance}, physical_device{physical_device}, device{device}, graphics_queue{graphics_queue}, window{window}, swap_chain{swap_chain}
{
    initializeImGui();
}

void UI::initializeImGui()
{
    createImGUIContext();
    createDescriptorPool();
    setupRendererBackends();
}

void UI::createImGUIContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
}

void UI::createDescriptorPool()
{
    descriptor_pool = DescriptorPoolBuilder(device)
        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
        .setMaxSets(1)
        .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
        .build();
}

namespace callbacks
{
    void checkResult(VkResult result)
    {
        if (result == 0)
        {
            return;
        }

        LogSystem::log(LogSeverity::ERROR, "[vulkan] Error: VkResult = ", result);
        if (result < 0)
        {
            abort();
        }
    }
}

void UI::setupRendererBackends()
{
    auto as_glfw_window = dynamic_cast<GLFWWindow*>(&WindowSystem::get());
    ImGui_ImplGlfw_InitForVulkan(as_glfw_window->getGFLWwindow(), true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance.getInstance();
    init_info.PhysicalDevice = physical_device.getPhysicalDevice();
    init_info.Device = device.handle();
    init_info.QueueFamily = graphics_queue.getQueueFamilyIndex();
    init_info.Queue = graphics_queue.getQueue();
    init_info.DescriptorPool = descriptor_pool->descriptorPool();
    init_info.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    init_info.ImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.UseDynamicRendering = true;
    init_info.ColorAttachmentFormat = swap_chain->getSwapChainImageFormat();
    init_info.CheckVkResultFn = callbacks::checkResult;
    ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);
}

UI::~UI()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::updateUIElements(FrameInfo& frame_info)
{
    startNewFrame();

    ImGui::Begin("Settings");

    ImGui::Checkbox("Pixelize", &pixelize);
    frame_info.pixelize = pixelize;

    ImGui::End();

    ImGui::Render();
}

void UI::startNewFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI::renderUIElements(VkCommandBuffer command_buffer, uint32_t current_frame_index) const
{
    VkClearValue clear_value{};
    clear_value.color = VkClearColorValue{.float32 = {0.1f, 0.1f, 0.1f, 1.0f}};

    VkRenderingAttachmentInfoKHR color_attachment_info{};
    color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    color_attachment_info.imageView = swap_chain->getImageView(current_frame_index);
    color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{.offset = {0, 0}, .extent = swap_chain->getSwapChainExtent()};
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);

    pvkCmdEndRenderingKHR(command_buffer);

    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    image_memory_barrier.image = swap_chain->getImage(current_frame_index);
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

void UI::handleWindowResize(SwapChain* new_swap_chain)
{
    assert(new_swap_chain && "Passed swap chain is null!");
    swap_chain = new_swap_chain;
}