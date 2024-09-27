#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"
#include <gmock/gmock-actions.h>

#include "Objects/Components/MeshComponent.h"

#include <Environment.h>
#include <Assets/AssetManager.h>

#include "Assets/Material/Material.h"
#include "Objects/Object.h"

using testing::Return;

struct MeshComponentTests : public ::testing::Test
{
    std::unique_ptr<AssetManager> asset_manager = std::make_unique<AssetManager>(VulkanSystem::get(), TestsEnvironment::vulkanMemoryAllocator());
    Object owner;

    void SetUp() override
    {
        owner = Object{};
    }
};

TEST_F(MeshComponentTests, shouldSetMeshToComponent)
{
    // given
    MeshComponent mesh_component{owner};
    Mesh* debug_mesh = asset_manager->fetchMesh(Assets::DEBUG_MESH_NAME);

    // when
    mesh_component.setMesh(debug_mesh);

    // then
    EXPECT_EQ(mesh_component.getMeshName(), debug_mesh->name);
    EXPECT_EQ(mesh_component.getModels(), debug_mesh->models);
    EXPECT_EQ(mesh_component.getMaterials(), debug_mesh->materials);
}

TEST_F(MeshComponentTests, shouldFindMaterial)
{
    // given
    MeshComponent mesh_component{owner};
    Mesh* debug_mesh = asset_manager->fetchMesh(Assets::DEBUG_MESH_NAME);
    mesh_component.setMesh(debug_mesh);

    // when
    Material* found_material = mesh_component.findMaterial(debug_mesh->materials[0]->getName());

    // then
    EXPECT_EQ(found_material, debug_mesh->materials[0]);
}

TEST_F(MeshComponentTests, shouldReturnMeshRequiredMaterials)
{
    // given
    MeshComponent mesh_component{owner};
    Mesh* debug_mesh = asset_manager->fetchMesh(Assets::DEBUG_MESH_NAME);
    mesh_component.setMesh(debug_mesh);

    // when
    const std::vector<std::string> required_materials = mesh_component.getRequiredMaterials();

    // then
    for (size_t i = 0; i < debug_mesh->materials.size(); ++i)
    {
        EXPECT_EQ(required_materials[i], debug_mesh->materials[i]->getName());
    }
}