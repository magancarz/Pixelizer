#include "gtest/gtest.h"

#include <Assets/MeshData.h>
#include <Assets/OBJ/OBJLoaderTests.h>

TEST(OBJLoaderTests, shouldCorrectlyLoadOBJData)
{
    // given
    std::string example_mesh_name = Assets::DEFAULT_MESH_NAME;

    // when
    MeshData mesh_data = OBJLoaderTests::loadMeshFromFile(example_mesh_name);

    // then
    EXPECT_EQ(mesh_data.name, example_mesh_name);

    EXPECT_EQ(mesh_data.materials_data.size(), 1);
    MaterialData material_data = mesh_data.materials_data.front();
    MaterialData default_material_data{};
    EXPECT_EQ(material_data.name, default_material_data.name);
    EXPECT_EQ(material_data.diffuse_texture_name, default_material_data.diffuse_texture_name);

    EXPECT_EQ(mesh_data.models_data.size(), 1);
    ModelData model_data = mesh_data.models_data.front();
    EXPECT_EQ(model_data.vertices.size(), 24);
    EXPECT_EQ(model_data.indices.size(), 36);
}

TEST(OBJLoaderTests, shouldReturnEmptyMeshDataIfFileDoesntExist)
{
    // given
    std::string example_non_existing_mesh_name{"non_existing_invalid_mesh_test"};

    // when
    MeshData empty_mesh_data = OBJLoaderTests::loadMeshFromFile(example_non_existing_mesh_name);

    // then
    EXPECT_EQ(empty_mesh_data.name, Assets::EMPTY_MESH_NAME);
    EXPECT_EQ(empty_mesh_data.materials_data.size(), 0);
    EXPECT_EQ(empty_mesh_data.models_data.size(), 0);
}