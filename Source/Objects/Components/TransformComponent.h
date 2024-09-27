#pragma once

#include <glm/ext/matrix_transform.hpp>

#include "Objects/Components/ObjectComponent.h"

class TransformComponent : public ObjectComponent
{
public:
    explicit TransformComponent(Object& owner);

    [[nodiscard]] glm::vec3 getLocation() const;
    void translate(const glm::vec3& translation);
    void setLocation(const glm::vec3& location);
    void setRelativeLocation(const glm::vec3&) override;

    [[nodiscard]] glm::vec3 getRotationInEulerAngles() const;
    void rotateWithEulerAngles(const glm::vec3& euler_angles);
    void setRotationInEulerAngles(const glm::vec3& euler_angles);

    void setScale(const glm::vec3& scale);

    [[nodiscard]] glm::mat4 transform() const;
    [[nodiscard]] glm::mat3 normalMatrix() const;

private:
    glm::vec3 translation{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 rotation{0.f};
};
