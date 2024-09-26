#include "gtest/gtest.h"

#include "Assets/Texture/TextureLoader.h"
#include "Assets/Defines.h"
#include "Assets/Texture/TextureData.h"

TEST(TextureLoaderTests, shouldCorrectlyLoadTextureData)
{
    // given
    std::string example_texture_name = "red.png";

    // when
    TextureData texture_data = TextureLoader::loadFromAssetFile(example_texture_name);

    // then
    EXPECT_EQ(texture_data.name, example_texture_name);
    EXPECT_EQ(texture_data.width, 16);
    EXPECT_EQ(texture_data.height, 16);

    std::array<uint8_t, TextureLoader::EXPECTED_NUMBER_OF_CHANNELS> pixel{136, 0, 21, 255};
    for (size_t i = 0; i < texture_data.width * texture_data.height * TextureLoader::EXPECTED_NUMBER_OF_CHANNELS; ++i)
    {
        EXPECT_EQ(texture_data.data[i], pixel[i % TextureLoader::EXPECTED_NUMBER_OF_CHANNELS]);
    }
}

TEST(TextureLoaderTests, shouldReturnEmptyTextureIfFileDoesntExist)
{
    // given
    std::string example_missing_texture_name = "__invalid_texture_name_gabagoo__";

    // when
    TextureData empty_texture_data = TextureLoader::loadFromAssetFile(example_missing_texture_name);

    // then
    EXPECT_EQ(empty_texture_data.name, Assets::EMPTY_TEXTURE_NAME);
    EXPECT_EQ(empty_texture_data.width, 0);
    EXPECT_EQ(empty_texture_data.height, 0);
    EXPECT_EQ(empty_texture_data.data.size(), 0);
}