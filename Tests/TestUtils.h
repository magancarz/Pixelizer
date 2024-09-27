#pragma once

#include <string>

#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "Environment.h"
#include "Assets/Texture/TextureLoader.h"
#include "Logs/LogSystem.h"
#include "RenderEngine/RenderingAPI/Memory/Buffer.h"

class TestUtils
{
public:
    static void deleteFileIfExists(const std::string& location);
    static bool fileExists(const std::string& file_location);
    static std::string loadFileToString(const std::string& file_location);

    static void printMatrix(const glm::mat4& matrix);
    static void printVector(const glm::vec3& vector);

    template <typename T>
    static void expectTwoValuesToBeEqual(T first_value, T second_value, double precision = 0.000001)
    {
        EXPECT_TRUE(abs(second_value - first_value) < precision);
    }

    static void expectTwoVectorsToBeEqual(const glm::vec3& actual_vector, const glm::vec3& expected_vector);
    static void expectTwoMatricesToBeEqual(const glm::mat4& actual_matrix, const glm::mat4& expected_matrix);
    static void expectTwoMatricesToBeEqual(const VkTransformMatrixKHR& actual_matrix, const VkTransformMatrixKHR& expected_matrix);

    static float randomFloat();
    static float randomFloat(float min, float max);
    static glm::mat4 randomTransform();

    template <typename T>
    static void expectBufferHasEqualData(const Buffer& buffer, const std::vector<T>& expected_data)
    {
        BufferInfo data_buffer_info{};
        data_buffer_info.instance_size = sizeof(T);
        data_buffer_info.instance_count = static_cast<uint32_t>(expected_data.size());
        data_buffer_info.usage_flags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        data_buffer_info.required_memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        data_buffer_info.allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        auto data_buffer = TestsEnvironment::vulkanMemoryAllocator().createBuffer(data_buffer_info);
        data_buffer->copyFrom(buffer);

        data_buffer->map();
        auto actual_data = std::bit_cast<T*>(data_buffer->getMappedMemory());
        for (size_t i = 0; i < expected_data.size(); ++i)
        {
            EXPECT_EQ(actual_data[i], expected_data[i]);
        }
    }

    static void failIfVulkanValidationLayersErrorsWerePresent();

    static void savePNGImage(
        const std::string& texture_location,
        const std::vector<unsigned char>& texture_data,
        uint32_t texture_width,
        uint32_t texture_height,
        uint32_t channels);

    static void expectTwoImagesToBeEqual(
        uint32_t compared_image_width,
        uint32_t compared_image_height,
        uint32_t compared_image_channels,
        const std::vector<unsigned char>& compared_image_data,
        const std::string& image_to_compare_location);

    static bool compareTwoImages(
        uint32_t compared_image_width,
        uint32_t compared_image_height,
        uint32_t compared_image_channels,
        const std::vector<unsigned char>& compared_image_data,
        const std::string& image_to_compare_location);

    template <typename T>
    static bool compareTwoImages(
        uint32_t image_width,
        uint32_t image_height,
        uint32_t channels,
        const std::vector<T>& first_image_data,
        const std::vector<T>& second_image_data,
        float accepted_result = 0.9f)
    {
        float result{0};

        for (size_t i = 0; i < image_height; ++i)
        {
            for (size_t j = 0; j < image_width; ++j)
            {
                for (size_t c = 0; c < channels; ++c)
                {
                    size_t index = i * image_width * channels + j * channels + c;
                    T first_image_value = first_image_data[index];
                    T second_image_value = second_image_data[index];
                    result += 1.0f - std::abs(static_cast<float>(first_image_value - second_image_value) / second_image_value);
                }
            }
        }

        result /= (image_width * image_height * channels);

        LogSystem::log(LogSeverity::VERBOSE, "Compared images have ", result, "% similar data");

        return result >= accepted_result;
    }
};
