#include "TransformComponent.h"

#include <cstdio>

TransformComponent::TransformComponent(Object& owner)
    : ObjectComponent(owner) {}

glm::vec3 TransformComponent::getLocation() const
{
    return translation;
}

void TransformComponent::translate(const glm::vec3& translation)
{
    if (translation == glm::vec3{0})
    {
        return;
    }

    this->translation += translation;
}

void TransformComponent::setLocation(const glm::vec3& location)
{
    if (location == translation)
    {
        return;
    }

    translation = location;
}

void TransformComponent::setRelativeLocation(const glm::vec3&)
{
    assert(false && "Use setLocation instead");
}

glm::vec3 TransformComponent::getRotationInEulerAngles() const
{
    return rotation;
}

void TransformComponent::rotateWithEulerAngles(const glm::vec3& euler_angles)
{
    if (euler_angles == glm::vec3{0})
    {
        return;
    }

    rotation += euler_angles;
}

void TransformComponent::setRotationInEulerAngles(const glm::vec3& euler_angles)
{
    if (euler_angles == rotation)
    {
        return;
    }

    rotation = euler_angles;
}

void TransformComponent::setScale(const glm::vec3& scale)
{
    this->scale = scale;
}

glm::mat4 TransformComponent::transform() const
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return
    {
        {
            scale.x * (c1 * c3 + s1 * s2 * s3),
            scale.x * (c2 * s3),
            scale.x * (c1 * s2 * s3 - c3 * s1),
            0.0f,
        },
        {
            scale.y * (c3 * s1 * s2 - c1 * s3),
            scale.y * (c2 * c3),
            scale.y * (c1 * c3 * s2 + s1 * s3),
            0.0f,
        },
        {
            scale.z * (c2 * s1),
            scale.z * (-s2),
            scale.z * (c1 * c2),
            0.0f,
        },
        {translation.x, translation.y, translation.z, 1.0f}
    };
}

glm::mat3 TransformComponent::normalMatrix() const
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 inv_scale = 1.0f / scale;

    return glm::mat3
    {
        {inv_scale.x * (c1 * c3 + s1 * s2 * s3), inv_scale.x * (c2 * s3), inv_scale.x * (c1 * s2 * s3 - c3 * s1)},
        {inv_scale.y * (c3 * s1 * s2 - c1 * s3), inv_scale.y * (c2 * c3), inv_scale.y * (c1 * c3 * s2 + s1 * s3)},
        {inv_scale.z * (c2 * s1), inv_scale.z * (-s2), inv_scale.z * (c1 * c2)},
    };
}