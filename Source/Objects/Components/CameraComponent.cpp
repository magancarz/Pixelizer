#include "CameraComponent.h"

#include <cmath>

#include "RenderEngine/FrameInfo.h"
#include "Objects/Object.h"

CameraComponent::CameraComponent(Object& owner, TransformComponent& transform_component)
    : ObjectComponent(owner), transform_component{transform_component} {}

void CameraComponent::update(FrameInfo& frame_info)
{
    setViewYXZ(transform_component.getLocation(), transform_component.getRotationInEulerAngles());

    frame_info.camera_view_matrix = view;
    frame_info.camera_projection_matrix = projection;
}

void CameraComponent::setPerspectiveProjection(float fovy, float aspect)
{
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tan_half_fov_y = glm::tan(fovy / 2.f);
    projection = glm::mat4{0.0f};
    projection[0][0] = 1.f / (aspect * tan_half_fov_y);
    projection[1][1] = -1.f / tan_half_fov_y;
    projection[2][2] = CAMERA_FAR / (CAMERA_FAR - CAMERA_NEAR);
    projection[2][3] = 1.f;
    projection[3][2] = -(CAMERA_FAR * CAMERA_NEAR) / (CAMERA_FAR - CAMERA_NEAR);
}

void CameraComponent::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    view = glm::mat4
    {
        {u.x, v.x, w.x, 0.0f},
        {u.y, v.y, w.y, 0.0f},
        {u.z, v.z, w.z, 0.0f},
        {-glm::dot(u, position), -glm::dot(v, position), -glm::dot(w, position), 1.0f}
    };

    inverse_view = glm::mat4
    {
        {u.x, u.y, u.z, 0.0f},
        {v.x, v.y, v.z, 0.0f},
        {w.x, w.y, w.z, 0.0f},
        {position.x, position.y, position.z, 1.0f}
    };
}