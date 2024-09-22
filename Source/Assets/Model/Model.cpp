#include "Model.h"

#include "ModelData.h"
#include "RenderEngine/RenderingAPI/Memory/VulkanMemoryAllocator.h"

Model::Model(
        VulkanMemoryAllocator& memory_allocator,
        const ModelData& model_data)
    : name{model_data.name}, required_material{model_data.required_material}
{
    createVertexBuffer(memory_allocator, model_data.vertices);
    createIndexBuffer(memory_allocator, model_data.indices);
}

void Model::createVertexBuffer(VulkanMemoryAllocator& memory_allocator, const std::vector<Vertex>& vertices)
{
    vertex_count = static_cast<uint32_t>(vertices.size());
    assert(vertex_count >= 3 && "Vertex count must be at least 3.");

    auto staging_buffer = memory_allocator.createStagingBuffer(
        sizeof(Vertex),
        static_cast<uint32_t>(vertices.size()),
        vertices.data());

    BufferInfo vertex_buffer_info{};
    vertex_buffer_info.instance_size = sizeof(Vertex);
    vertex_buffer_info.instance_count = static_cast<uint32_t>(vertices.size());
    vertex_buffer_info.usage_flags =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    vertex_buffer = memory_allocator.createBuffer(vertex_buffer_info);
    vertex_buffer->copyFrom(*staging_buffer);
}

void Model::createIndexBuffer(VulkanMemoryAllocator& memory_allocator, const std::vector<uint32_t>& indices)
{
    index_count = static_cast<uint32_t>(indices.size());

    auto staging_buffer = memory_allocator.createStagingBuffer(
        sizeof(uint32_t),
        static_cast<uint32_t>(indices.size()),
        indices.data());

    BufferInfo index_buffer_info{};
    index_buffer_info.instance_size = sizeof(uint32_t);
    index_buffer_info.instance_count = static_cast<uint32_t>(indices.size());
    index_buffer_info.usage_flags =
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    index_buffer = memory_allocator.createBuffer(index_buffer_info);
    index_buffer->copyFrom(*staging_buffer);
}

void Model::bind(VkCommandBuffer command_buffer)
{
    VkBuffer buffers[] = {vertex_buffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Model::draw(VkCommandBuffer command_buffer)
{
    vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
}

ModelDescription Model::getModelDescription() const
{
    assert(vertex_buffer && index_buffer && index_count >= 3 && "Model should be valid!");

    ModelDescription model_description{};
    model_description.vertex_buffer = vertex_buffer.get();
    model_description.index_buffer = index_buffer.get();
    model_description.num_of_triangles = index_count / 3;
    model_description.num_of_vertices = vertex_count;
    model_description.num_of_indices = index_count;

    return model_description;
}
