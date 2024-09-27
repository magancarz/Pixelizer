#pragma once

#include <glm/glm.hpp>

#include "ObjectComponent.h"
#include "TransformComponent.h"

class CameraComponent : public ObjectComponent
{
public:
    CameraComponent(Object& owner, TransformComponent& transform_component, float arm_length = 10.0f);

    void update(FrameInfo& frame_info) override;

    void setPerspectiveProjection(float fovy, float aspect);
    void setViewYXZ(glm::vec3 rotation);

    [[nodiscard]] const glm::mat4& getProjection() const { return projection; }
    [[nodiscard]] const glm::mat4& getView() const { return view; }
    [[nodiscard]] float getArmLength() const { return arm_length; }

    static constexpr float CAMERA_NEAR = 0.1f;
    static constexpr float CAMERA_FAR = 100.0f;
    static constexpr glm::vec3 CAMERA_FRONT{0, 0, 1};
    static constexpr glm::vec3 CAMERA_UP{0, 1, 0};

private:
    TransformComponent& transform_component;

    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};

    float arm_length{10.0f};
};
