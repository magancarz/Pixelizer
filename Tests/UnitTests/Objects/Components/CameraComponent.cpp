#include "gtest/gtest.h"

#include <glm/ext/matrix_clip_space.hpp>

#include "Objects/Components/CameraComponent.h"
#include "Objects/Components/TransformComponent.h"
#include "Objects/Object.h"
#include "RenderEngine/FrameInfo.h"

#include "TestUtils.h"

struct CameraComponentTests : public ::testing::Test
{
    Object owner;

    void SetUp() override
    {
        owner = Object{};
        owner.addComponent(std::make_unique<TransformComponent>(owner));
    }
};

TEST_F(CameraComponentTests, shouldReturnCorrectProjectionMatrix)
{
    // given
    CameraComponent camera_component{owner, *owner.findComponentByClass<TransformComponent>()};
    constexpr float fov_y = 70.f;
    constexpr float aspect = 1280.f / 800.f;
    camera_component.setPerspectiveProjection(fov_y, aspect);

    glm::mat4 expected_projection = glm::perspective(glm::radians(fov_y), aspect, CameraComponent::CAMERA_NEAR, CameraComponent::CAMERA_FAR);
    expected_projection[1][1] *= -1;

    // when
    glm::mat4 projection = camera_component.getProjection();

    // then
    TestUtils::expectTwoMatricesToBeEqual(projection, expected_projection);
}

TEST_F(CameraComponentTests, shouldReturnCorrectViewMatrix)
{
    // given
    CameraComponent camera_component{owner, *owner.findComponentByClass<TransformComponent>()};
    constexpr glm::vec3 camera_rotation{glm::radians(55.f), glm::radians(90.f), 0};
    camera_component.setViewYXZ(camera_rotation);

    glm::mat4 rotation_matrix = glm::rotate(glm::mat4{1.0f}, camera_rotation.x, glm::vec3{1, 0, 0});
    rotation_matrix = glm::rotate(rotation_matrix, camera_rotation.y, glm::vec3{0, -1, 0});
    const glm::vec3 forward_direction = CameraComponent::CAMERA_FRONT * glm::mat3{rotation_matrix};
    const glm::vec3 backward_direction = -forward_direction;
    const glm::mat4 expected_view = glm::lookAt(camera_component.getArmLength() * backward_direction, glm::vec3{0}, CameraComponent::CAMERA_UP);

    // when
    glm::mat4 view = camera_component.getView();

    // then
    TestUtils::expectTwoMatricesToBeEqual(view, expected_view);
}

TEST_F(CameraComponentTests, shouldUpdateProjectionMatrixInfoForEveryFrame)
{
    // given
    CameraComponent camera_component{owner, *owner.findComponentByClass<TransformComponent>()};
    constexpr float fov_y = 70.f;
    constexpr float aspect = 1280.f / 800.f;
    camera_component.setPerspectiveProjection(fov_y, aspect);

    glm::mat4 expected_projection = glm::perspective(glm::radians(fov_y), aspect, CameraComponent::CAMERA_NEAR, CameraComponent::CAMERA_FAR);
    expected_projection[1][1] *= -1;

    FrameInfo frame_info{};

    // when
    camera_component.update(frame_info);

    // then
    TestUtils::expectTwoMatricesToBeEqual(frame_info.camera_projection_matrix, expected_projection);
}
