#include "TestUtils.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <random>

#include <TestLogger.h>
#include <glm/ext/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void TestUtils::deleteFileIfExists(const std::string& location)
{
    if (fileExists(location))
    {
        std::filesystem::remove(location);
        std::cout << "File at location " << location << " removed successfully\n";
    }
}

bool TestUtils::fileExists(const std::string& file_location)
{
    std::filesystem::path canonical_path = std::filesystem::canonical(file_location);
    if (std::filesystem::exists(canonical_path))
    {
        return true;
    }

    std::cout << "Couldn't find any file at location " << file_location << "\n";
    return false;
}

std::string TestUtils::loadFileToString(const std::string& file_location)
{
    if (!fileExists(file_location))
    {
        return "";
    }

    std::ifstream file(file_location);
    if (!file.is_open())
    {
        std::cerr << "Error while opening file " << file_location << "\n";
        std::cout << "Current working directory is " << std::filesystem::current_path() << "\n";
        return "";
    }

    std::string content;
    std::string line;
    while (std::getline(file, line))
    {
        content += line + "\n";
    }

    file.close();
    return content;
}

void TestUtils::printMatrix(const glm::mat4& matrix)
{
    printf("%f, %f, %f, %f\n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
    printf("%f, %f, %f, %f\n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
    printf("%f, %f, %f, %f\n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
    printf("%f, %f, %f, %f\n\n", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
}

void TestUtils::printVector(const glm::vec3& vector)
{
    printf("%f, %f, %f\n", vector[0], vector[1], vector[2]);
}

void TestUtils::expectTwoVectorsToBeEqual(const glm::vec3& actual_vector, const glm::vec3& expected_vector)
{
    for (int i = 0; i < 2; ++i)
    {
        expectTwoValuesToBeEqual(actual_vector[i], expected_vector[i]);
    }
}

void TestUtils::expectTwoMatricesToBeEqual(const glm::mat4& actual_matrix, const glm::mat4& expected_matrix)
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            expectTwoValuesToBeEqual(actual_matrix[i][j], expected_matrix[i][j]);
        }
    }
}

void TestUtils::expectTwoMatricesToBeEqual(const VkTransformMatrixKHR& actual_matrix, const VkTransformMatrixKHR& expected_matrix)
{
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            expectTwoValuesToBeEqual(actual_matrix.matrix[i][j], expected_matrix.matrix[i][j]);
        }
    }
}

float TestUtils::randomFloat()
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution dist(0.0f, 1.0f);

    return dist(mt);
}

float TestUtils::randomFloat(float min, float max)
{
    return std::lerp(min, max, randomFloat());
}

glm::mat4 TestUtils::randomTransform()
{
    const glm::vec3 random_translation{randomFloat(-10.0f, 10.0f), randomFloat(-10.0f, 10.0f), randomFloat(-10.0f, 10.0f)};
    const glm::vec3 random_rotation{randomFloat(-180.0f, 180.0f), randomFloat(-180.0f, 180.0f), randomFloat(-180.0f, 180.0f)};
    const glm::vec3 random_scale{randomFloat(-10.0, 10.0f), randomFloat(-10.0, 10.0f), randomFloat(-10.0, 10.0f)};

    const float c3 = glm::cos(random_rotation.z);
    const float s3 = glm::sin(random_rotation.z);
    const float c2 = glm::cos(random_rotation.x);
    const float s2 = glm::sin(random_rotation.x);
    const float c1 = glm::cos(random_rotation.y);
    const float s1 = glm::sin(random_rotation.y);

    return glm::mat4
    {
        {random_scale.x * (c1 * c3 + s1 * s2 * s3), random_scale.x * (c2 * s3), random_scale.x * (c1 * s2 * s3 - c3 * s1), 0.0f},
        {random_scale.y * (c3 * s1 * s2 - c1 * s3), random_scale.y * (c2 * c3), random_scale.y * (c1 * c3 * s2 + s1 * s3), 0.0f},
        {random_scale.z * (c2 * s1), random_scale.z * (-s2), random_scale.z * (c1 * c2), 0.0f},
        {random_translation.x, random_translation.y, random_translation.z, 1.0f}
    };
}

void TestUtils::failIfVulkanValidationLayersErrorsWerePresent()
{
    if (TestsEnvironment::testLogger().anyVulkanValidationLayersErrors())
    {
        GTEST_FATAL_FAILURE_("There were vulkan validation layers errors during test!");
    }
}

void TestUtils::savePNGImage(
        const std::string& texture_location,
        const std::vector<unsigned char>& texture_data,
        uint32_t texture_width,
        uint32_t texture_height,
        uint32_t channels)
{
    stbi_write_png(
        texture_location.c_str(),
        texture_width,
        texture_height,
        channels,
        texture_data.data(),
        texture_width * channels);
}

void TestUtils::expectTwoImagesToBeEqual(
        uint32_t compared_image_width,
        uint32_t compared_image_height,
        uint32_t compared_image_channels,
        const std::vector<unsigned char>& compared_image_data,
        const std::string& image_to_compare_location)
{
    EXPECT_TRUE(compareTwoImages(compared_image_width, compared_image_height, compared_image_channels, compared_image_data, image_to_compare_location));
}

bool TestUtils::compareTwoImages(
        uint32_t compared_image_width,
        uint32_t compared_image_height,
        uint32_t compared_image_channels,
        const std::vector<unsigned char>& compared_image_data,
        const std::string& image_to_compare_location)
{
    TextureData image_to_compare_with_data = TextureLoader::loadFromAssetFile(image_to_compare_location);

    if (image_to_compare_with_data.width != compared_image_width ||
        image_to_compare_with_data.height != compared_image_height ||
        image_to_compare_with_data.number_of_channels != compared_image_channels)
    {
        return false;
    }

    return compareTwoImages(
        compared_image_width,
        compared_image_height,
        compared_image_channels,
        compared_image_data,
        image_to_compare_with_data.data);
}
