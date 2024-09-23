#pragma once

#include "MaterialInfo.h"
#include "RenderEngine/RenderingAPI/Descriptors/DescriptorPool.h"
#include "RenderEngine/RenderingAPI/Descriptors/DescriptorSetLayout.h"
#include "RenderEngine/RenderingAPI/Textures/Texture.h"

class VulkanMemoryAllocator;

class Material
{
public:
    static void initializeMaterials(Device& logical_device);

    Material(
        Device& logical_device,
        const MaterialInfo& in_material_info);

    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] Texture* getDiffuseTexture() const { return diffuse_texture; }

    static DescriptorSetLayout& getMaterialDescriptorSetLayout() { return *diffuse_material_descriptor_set_layout; }
    [[nodiscard]] const VkDescriptorSet& getMaterialDescriptorSet() const { return material_descriptor_set; }

protected:
    Device& logical_device;

    static VkDescriptorSet createMaterialDescriptorSet(Texture* diffuse_texture);

    inline static std::unique_ptr<DescriptorPool> descriptor_pool;
    inline static std::unique_ptr<DescriptorSetLayout> diffuse_material_descriptor_set_layout;

    std::string name{};
    Texture* diffuse_texture{nullptr};
    VkDescriptorSet material_descriptor_set{VK_NULL_HANDLE};
};
