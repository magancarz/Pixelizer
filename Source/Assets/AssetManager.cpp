#include "AssetManager.h"

#include "Logs/LogSystem.h"
#include "OBJ/OBJLoaderTests.h"
#include "Texture/TextureInfo.h"
#include "Texture/TextureLoader.h"
#include "RenderEngine/RenderingAPI/Textures/Texture.h"

AssetManager::AssetManager(VulkanSystem& vulkan_facade, VulkanMemoryAllocator& memory_allocator)
    : vulkan_facade{vulkan_facade}, memory_allocator{memory_allocator}
{
    Material::initializeMaterials(vulkan_facade.getLogicalDevice());
}

Mesh* AssetManager::fetchMesh(const std::string& mesh_name)
{
    LogSystem::log(LogSeverity::LOG, "Fetching mesh named ", mesh_name.c_str());
    if (available_meshes.contains(mesh_name))
    {
        LogSystem::log(LogSeverity::LOG, "Mesh found in available meshes list. Returning...");
        return available_meshes[mesh_name].get();
    }

    LogSystem::log(LogSeverity::LOG, "Mesh was not found in available meshes list. Loading from file...");
    return storeMesh(OBJLoaderTests::loadMeshFromFile(mesh_name));
}

Mesh* AssetManager::storeMesh(const MeshData& mesh_data)
{
    if (available_meshes.contains(mesh_data.name))
    {
        LogSystem::log(LogSeverity::WARNING, "Tried to store mesh ", mesh_data.name, " that already existed in asset manager!");
        return available_meshes.at(mesh_data.name).get();
    }

    auto mesh = std::make_unique<Mesh>();
    mesh->name = mesh_data.name;
    mesh->materials.reserve(mesh_data.materials_data.size());
    for (auto& material_data : mesh_data.materials_data)
    {
        mesh->materials.emplace_back(storeMaterial(material_data));
    }

    mesh->models.reserve(mesh_data.models_data.size());
    for (auto& model_data : mesh_data.models_data)
    {
        mesh->models.emplace_back(storeModel(model_data));
    }

    available_meshes.try_emplace(mesh_data.name, std::move(mesh));
    return available_meshes[mesh_data.name].get();
}

Material* AssetManager::storeMaterial(const MaterialData& material_data)
{
    if (available_materials.contains(material_data.name))
    {
        LogSystem::log(LogSeverity::WARNING, "Tried to store material ", material_data.name, " that already existed in asset manager!");
        return available_materials.at(material_data.name).get();
    }

    MaterialInfo material_info{};
    material_info.name = material_data.name;
    material_info.diffuse_texture = fetchTexture(material_data.diffuse_texture_name);
    available_materials.try_emplace(material_data.name, std::make_unique<Material>(vulkan_facade.getLogicalDevice(), material_info));
    return available_materials[material_data.name].get();
}

Model* AssetManager::storeModel(const ModelData& model_data)
{
    if (available_models.contains(model_data.name))
    {
        LogSystem::log(LogSeverity::WARNING, "Tried to store material ", model_data.name, " that already existed in asset manager!");
        return available_models.at(model_data.name).get();
    }

    available_models.try_emplace(model_data.name, std::make_unique<Model>(memory_allocator, model_data));
    return available_models[model_data.name].get();
}

Texture* AssetManager::fetchTexture(const std::string& texture_name)
{
    LogSystem::log(LogSeverity::LOG, "Fetching texture named ", texture_name.c_str());
    if (available_textures.contains(texture_name))
    {
        LogSystem::log(LogSeverity::LOG, "Texture found in available textures list. Returning...");
        return available_textures[texture_name].get();
    }

    LogSystem::log(LogSeverity::LOG, "Texture was not found in available textures list. Loading from file...");
    return storeTexture(TextureLoader::loadFromAssetFile(texture_name));
}

Texture* AssetManager::storeTexture(const TextureData& texture_data)
{
    if (available_textures.contains(texture_data.name))
    {
        LogSystem::log(LogSeverity::WARNING, "Tried to store texture ", texture_data.name, " that already existed in asset manager!");
        return available_textures.at(texture_data.name).get();
    }

    TextureInfo texture_info{};
    texture_info.name = texture_data.name;
    texture_info.width = texture_data.width;
    texture_info.height = texture_data.height;
    texture_info.format = texture_data.format;
    texture_info.mip_levels = texture_data.mip_levels;

    auto texture = std::make_unique<Texture>
    (
        vulkan_facade.getPhysicalDevice(),
        vulkan_facade.getLogicalDevice(),
        vulkan_facade.getTransferCommandPool(),
        memory_allocator,
        texture_info
    );
    texture->writeTextureData(texture_data.data);

    available_textures.try_emplace(texture_data.name, std::move(texture));

    return available_textures.at(texture_data.name).get();
}
