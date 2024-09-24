#include "Renderer.h"

#include "imgui.h"

#include "RenderEngine/RenderingAPI/Descriptors/DescriptorWriter.h"
#include "RenderingAPI/Pipeline.h"
#include "RenderingAPI/PipelineConfigInfo.h"
#include "RenderingAPI/VulkanDefines.h"
#include "RenderingAPI/VulkanUtils.h"
#include "RenderingAPI/CommandBuffer/CommandBuffer.h"
#include "RenderingAPI/Descriptors/DescriptorPoolBuilder.h"
#include "RenderingAPI/Descriptors/DescriptorSetLayoutBuilder.h"
#include "Common/CameraUBO.h"
#include "Common/PushConstantData.h"
#include "Common/PostProcessingPushConstantData.h"

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
    createCameraDescriptorSet();
    createRenderedToTextures();
    createSimplePipeline();
    createPostProcessingPipeline();
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
        swap_chain = std::make_unique<SwapChain>(surface, physical_device, device, memory_allocator, extent);
    }
    else
    {
        std::shared_ptr<SwapChain> old_swap_chain = std::move(swap_chain);
        swap_chain = std::make_unique<SwapChain>(surface, physical_device, device, memory_allocator, extent, old_swap_chain);

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

void Renderer::createCameraDescriptorSet()
{
    descriptor_pool = DescriptorPoolBuilder(device)
        .setMaxSets(16)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4)
        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 12)
        .build();

    camera_descriptor_set_layout = DescriptorSetLayoutBuilder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .build();

    BufferInfo camera_uniform_buffer_info{};
    camera_uniform_buffer_info.instance_size = sizeof(CameraUBO);
    camera_uniform_buffer_info.instance_count = 1;
    camera_uniform_buffer_info.usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    camera_uniform_buffer_info.required_memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    camera_uniform_buffer_info.allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    for (std::size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        camera_uniform_buffers[i] = memory_allocator.createBuffer(camera_uniform_buffer_info);
        VkDescriptorBufferInfo camera_descriptor_buffer_info = camera_uniform_buffers[i]->descriptorInfo();

        DescriptorWriter(*camera_descriptor_set_layout, *descriptor_pool)
            .writeBuffer(0, &camera_descriptor_buffer_info)
            .build(camera_descriptor_set_handles[i]);
    }
}

void Renderer::createRenderedToTextures()
{
    TextureInfo rendered_to_texture_info{};
    rendered_to_texture_info.width = window.getExtent().width;
    rendered_to_texture_info.height = window.getExtent().height;
    rendered_to_texture_info.format = VK_FORMAT_B8G8R8A8_SRGB;
    rendered_to_texture_info.mip_levels = 1;

    for (std::size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        rendered_to_textures.emplace_back(std::make_unique<Texture>(physical_device, device, graphics_command_pool, memory_allocator, rendered_to_texture_info));
    }
}

void Renderer::createSimplePipeline()
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(PushConstantData);

    std::vector<VkDescriptorSetLayout> descriptor_set_layouts
    {
        camera_descriptor_set_layout->getDescriptorSetLayout(),
        Material::getMaterialDescriptorSetLayout().getDescriptorSetLayout()
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();

    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(device.handle(), &pipeline_layout_info, VulkanDefines::NO_CALLBACK, &simple_pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    assert(simple_pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    VkFormat swap_chain_image_format = swap_chain->getSwapChainImageFormat();

    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info{};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &swap_chain_image_format;
    pipeline_rendering_create_info.depthAttachmentFormat = swap_chain->getSwapChainDepthImageFormat();

    PipelineConfigInfo config_info{};
    Pipeline::defaultPipelineConfigInfo(config_info);
    config_info.pipeline_layout = simple_pipeline_layout;
    config_info.rendering_create_info = pipeline_rendering_create_info;

    simple_pipeline = std::make_unique<Pipeline>
    (
        device,
        simple_pipeline_layout,
        "Shaders/SimpleShader.vert.spv",
        "Shaders/SimpleShader.frag.spv",
        config_info
    );
}

void Renderer::createPostProcessingPipeline()
{
    post_processed_image_descriptor_set_layout = DescriptorSetLayoutBuilder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .build();

    for (std::size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkDescriptorImageInfo descriptor_image_info = rendered_to_textures[i]->descriptorInfo();
        descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DescriptorWriter(*post_processed_image_descriptor_set_layout, *descriptor_pool)
            .writeImage(0, &descriptor_image_info)
            .build(post_processed_image_descriptor_set_handles[i]);
    }

    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(PostProcessingPushConstantData);

    std::vector<VkDescriptorSetLayout> descriptor_set_layouts{ post_processed_image_descriptor_set_layout->getDescriptorSetLayout() };

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();

    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(device.handle(), &pipeline_layout_info, VulkanDefines::NO_CALLBACK, &post_processing_pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    assert(post_processing_pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    VkFormat swap_chain_image_format = swap_chain->getSwapChainImageFormat();

    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info{};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &swap_chain_image_format;

    PipelineConfigInfo config_info{};
    Pipeline::defaultPipelineConfigInfo(config_info);
    config_info.pipeline_layout = post_processing_pipeline_layout;
    config_info.rendering_create_info = pipeline_rendering_create_info;
    config_info.binding_descriptions.clear();
    config_info.attribute_descriptions.clear();

    post_processing_pipeline = std::make_unique<Pipeline>
    (
        device,
        post_processing_pipeline_layout,
        "Shaders/PixelizeProcessor.vert.spv",
        "Shaders/PixelizeProcessor.frag.spv",
        config_info
    );
}

void Renderer::render(FrameInfo& frame_info)
{
    if (auto command_buffer = beginFrame())
    {
        frame_info.graphics_command_buffer = command_buffer;
        frame_info.window_size = swap_chain->getSwapChainExtent();
        frame_info.frame_index = swap_chain->getCurrentFrameIndex();

        renderModel(frame_info);
        applyPostProcessing(frame_info);

        endFrame(command_buffer);
    }
}

void Renderer::renderModel(FrameInfo& frame_info)
{
    beginRenderingModelRenderPass(frame_info.graphics_command_buffer);

    simple_pipeline->bind(frame_info.graphics_command_buffer);

    CameraUBO camera_ubo{};
    camera_ubo.view = frame_info.camera_view_matrix;
    camera_ubo.projection = frame_info.camera_projection_matrix;
    camera_uniform_buffers[current_image_index]->writeToBuffer(&camera_ubo);
    simple_pipeline->bindDescriptorSets(frame_info.graphics_command_buffer, &camera_descriptor_set_handles[current_image_index], 0, 1);

    PushConstantData push_constant_data{};
    push_constant_data.model = frame_info.model_matrix;
    push_constant_data.normal = frame_info.normal_matrix;
    simple_pipeline->pushConstants(frame_info.graphics_command_buffer, VK_SHADER_STAGE_VERTEX_BIT, 0, &push_constant_data);

    Model* rendered_model = frame_info.rendered_model->models.front();
    rendered_model->bind(frame_info.graphics_command_buffer);

    Material* rendered_material = frame_info.rendered_model->materials.front();
    simple_pipeline->bindDescriptorSets(frame_info.graphics_command_buffer, &rendered_material->getMaterialDescriptorSet(), 1, 1);

    rendered_model->draw(frame_info.graphics_command_buffer);

    endRenderingModelRenderPass(frame_info.graphics_command_buffer);
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

void Renderer::beginRenderingModelRenderPass(VkCommandBuffer command_buffer)
{
    assert(is_frame_in_progress && "Can't call beginSwapChainRenderPass if frame is not in progress!");
    assert(command_buffer == getCurrentCommandBuffer().handle() &&
        "Can't begin updateElements pass on command buffer from a different frame!");

    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.image = rendered_to_textures[current_image_index]->getImage();
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
    color_attachment_info.imageView = rendered_to_textures[current_image_index]->getImageView();
    color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_info.clearValue = clear_value;

    VkClearValue depth_clear_value{};
    depth_clear_value.depthStencil = VkClearDepthStencilValue{.depth = 1.0f, .stencil = 0};

    VkRenderingAttachmentInfoKHR depth_attachment_info{};
    depth_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    depth_attachment_info.imageView = swap_chain->getDepthImageView(current_image_index);
    depth_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment_info.clearValue = depth_clear_value;

    VkRect2D render_area{};
    render_area.offset = {0, 0};
    render_area.extent = swap_chain->getSwapChainExtent();

    VkRenderingInfoKHR render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    render_info.renderArea = render_area;
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment_info;
    render_info.pDepthAttachment = &depth_attachment_info;

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

void Renderer::endRenderingModelRenderPass(VkCommandBuffer command_buffer)
{
    assert(is_frame_in_progress && "Can't call endSwapChainRenderPass if frame is not in progress!");
    assert(command_buffer == getCurrentCommandBuffer().handle() && "Can't end updateElements pass on command buffer from a different frame!");

    pvkCmdEndRenderingKHR(command_buffer);

    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_memory_barrier.image = rendered_to_textures[current_image_index]->getImage();
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
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &image_memory_barrier
    );
}

void Renderer::applyPostProcessing(FrameInfo& frame_info)
{
    beginPostProcessingRenderPass(frame_info.graphics_command_buffer);

    post_processing_pipeline->bind(frame_info.graphics_command_buffer);

    post_processing_pipeline->bindDescriptorSets(frame_info.graphics_command_buffer, &post_processed_image_descriptor_set_handles[frame_info.frame_index], 0, 1);

    vkCmdDraw(frame_info.graphics_command_buffer, 3, 2, 0, 0);

    endPostProcessingRenderPass(frame_info.graphics_command_buffer);
}

void Renderer::beginPostProcessingRenderPass(VkCommandBuffer command_buffer)
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

void Renderer::endPostProcessingRenderPass(VkCommandBuffer command_buffer)
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
