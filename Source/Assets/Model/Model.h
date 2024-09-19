#pragma once

#include <memory>

#include "RenderEngine/RenderingAPI/Memory/Buffer.h"
#include "ModelMemoryDescription.h"

struct ModelData;
struct ModelInfo;
struct Vertex;
class VulkanMemoryAllocator;

class Model
{
public:
    Model(
        VulkanMemoryAllocator& memory_allocator,
        const ModelData& model_data);
    virtual ~Model() = default;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] bool isMaterialRequired() const { return !required_material.empty(); }
    [[nodiscard]] std::string getRequiredMaterial() const { return required_material; }

    [[nodiscard]] ModelDescription getModelDescription() const;

    //TODO: remove later
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

protected:
    std::string name{};
    std::string required_material{};

    void createVertexBuffer(VulkanMemoryAllocator& memory_allocator, const std::vector<Vertex>& vertices);
    void createIndexBuffer(VulkanMemoryAllocator& memory_allocator, const std::vector<uint32_t>& indices);

    std::unique_ptr<Buffer> vertex_buffer;
    uint32_t vertex_count{0};

    std::unique_ptr<Buffer> index_buffer;
    uint32_t index_count{0};
};
