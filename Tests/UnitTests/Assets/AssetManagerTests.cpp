#include "gtest/gtest.h"

#include "Assets/AssetManager.h"

#include <Environment.h>

#include "Assets/Defines.h"
#include "Assets/Model/ModelData.h"

TEST(AssetManagerTests, shouldFetchMesh)
{
    // given
    AssetManager asset_manager{VulkanSystem::get(), TestsEnvironment::vulkanMemoryAllocator()};
    const std::string mesh_name{"suzanne"};

    // when
    Mesh* mesh = asset_manager.fetchMesh(mesh_name);

    // then
    EXPECT_EQ(mesh->name, mesh_name);

    EXPECT_EQ(mesh->models.size(), 1);
    EXPECT_EQ(mesh->models[0]->getName(), mesh_name);

    EXPECT_EQ(mesh->materials.size(), 1);
    EXPECT_EQ(mesh->materials[0]->getName(), "white");
}

TEST(AssetManagerTests, shouldFetchTexture)
{
    // given
    AssetManager asset_manager{VulkanSystem::get(), TestsEnvironment::vulkanMemoryAllocator()};

    // when
    const Texture* texture = asset_manager.fetchTexture(Assets::DEFAULT_DIFFUSE_TEXTURE);

    // then
    EXPECT_EQ(texture->getName(), Assets::DEFAULT_DIFFUSE_TEXTURE);
}
