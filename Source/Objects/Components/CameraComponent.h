#pragma once

#include <glm/glm.hpp>

#include "ObjectComponent.h"
#include "TransformComponent.h"

class CameraComponent : public ObjectComponent
{
public:
    CameraComponent(Object& owner, TransformComponent& transform_component);

    void update(FrameInfo& frame_info) override;

    void setPerspectiveProjection(float fovy, float aspect);
    void setCameraView(glm::vec3 rotation);

    [[nodiscard]] const glm::mat4& getProjection() const { return projection; }
    [[nodiscard]] const glm::mat4& getView() const { return view; }

    static constexpr float CAMERA_NEAR = 0.1f;
    static constexpr float CAMERA_FAR = 100.0f;

private:
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};

    static constexpr glm::vec3 CAMERA_FRONT{0, 0, 1};
    float arm_length{10.0f};

    TransformComponent& transform_component;
};
