#include "Material.h"

#include "RenderEngine/RenderingAPI/Descriptors/DescriptorPoolBuilder.h"
#include "RenderEngine/RenderingAPI/Descriptors/DescriptorSetLayoutBuilder.h"
#include "RenderEngine/RenderingAPI/Descriptors/DescriptorWriter.h"

void Material::initializeMaterials(Device& logical_device)
{
    if (!descriptor_pool)
    {
        descriptor_pool = DescriptorPoolBuilder(logical_device)
            .setMaxSets(10)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10)
            .build();
    }

    if (!diffuse_material_descriptor_set_layout)
    {
        diffuse_material_descriptor_set_layout = DescriptorSetLayoutBuilder(logical_device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();
    }
}

Material::Material(Device& logical_device, const MaterialInfo& material_info)
    : logical_device{logical_device}, name{material_info.name}, diffuse_texture{material_info.diffuse_texture},
    material_descriptor_set{createMaterialDescriptorSet(diffuse_texture)} {}

VkDescriptorSet Material::createMaterialDescriptorSet(Texture* diffuse_texture)
{
    VkDescriptorImageInfo diffuse_texture_image_info = diffuse_texture->descriptorInfo();

    VkDescriptorSet material_descriptor_set{VK_NULL_HANDLE};
    DescriptorWriter(*diffuse_material_descriptor_set_layout, *descriptor_pool)
        .writeImage(0, &diffuse_texture_image_info)
        .build(material_descriptor_set);

    return material_descriptor_set;
}
