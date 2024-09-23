#pragma once

#include <memory>
#include <unordered_map>

#include "RenderEngine/RenderingAPI/VulkanSystem.h"
#include "Assets/Material/Material.h"
#include "Model/Model.h"
#include "Mesh.h"
#include "MeshData.h"
#include "Material/MaterialData.h"

class Texture;

class AssetManager
{
public:
    AssetManager(VulkanSystem& vulkan_facade, VulkanMemoryAllocator& memory_allocator);
    virtual ~AssetManager() = default;

    AssetManager(const AssetManager&) = delete;
    AssetManager operator=(const AssetManager&) = delete;

    virtual Mesh* fetchMesh(const std::string& mesh_name);

    virtual Model* fetchModel(const std::string& model_name);

    virtual Material* fetchMaterial(const std::string& material_name);
    virtual std::vector<Material*> fetchRequiredMaterials(const std::vector<std::string>& required_materials);

    virtual Texture* fetchTexture(const std::string& texture_name);

protected:
    VulkanSystem& vulkan_facade;
    VulkanMemoryAllocator& memory_allocator;

    Mesh* storeMesh(const MeshData& mesh_data);

    std::unordered_map<std::string, std::unique_ptr<Mesh>> available_meshes;

    Model* storeModel(const ModelData& model_data);

    std::unordered_map<std::string, std::unique_ptr<Model>> available_models;

    Material* storeMaterial(const MaterialData& material_data);

    std::unordered_map<std::string, std::unique_ptr<Material>> available_materials;

    Texture* storeTexture(const TextureData& texture_data);

    std::unordered_map<std::string, std::unique_ptr<Texture>> available_textures;
};
