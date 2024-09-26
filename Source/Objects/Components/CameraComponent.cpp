#include "CameraComponent.h"

#include <cmath>

#include "glm/ext/matrix_clip_space.hpp"
#include "RenderEngine/FrameInfo.h"
#include "Objects/Object.h"

CameraComponent::CameraComponent(Object& owner, TransformComponent& transform_component, float arm_length)
    : ObjectComponent(owner), transform_component{transform_component}, arm_length{arm_length} {}

void CameraComponent::update(FrameInfo& frame_info)
{
    setViewYXZ(transform_component.getRotationInEulerAngles());

    frame_info.camera_view_matrix = view;
    frame_info.camera_projection_matrix = projection;
}

void CameraComponent::setPerspectiveProjection(float fov_y, float aspect)
{
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    projection = glm::perspective(glm::radians(fov_y), aspect, CAMERA_NEAR, CAMERA_FAR);
    projection[1][1] *= -1;
}

void CameraComponent::setViewYXZ(glm::vec3 rotation)
{
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4{1.0f}, rotation.x, glm::vec3{1, 0, 0});
    rotation_matrix = glm::rotate(rotation_matrix, rotation.y, glm::vec3{0, -1, 0});
    const glm::vec3 forward_direction = CAMERA_FRONT * glm::mat3{rotation_matrix};
    const glm::vec3 backward_direction = -forward_direction;
    view = glm::lookAt(arm_length * backward_direction, glm::vec3{0}, glm::vec3{0, 1, 0});
}