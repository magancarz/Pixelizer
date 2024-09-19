#pragma once

#include <glm/glm.hpp>

#include "ObjectComponent.h"

class CameraComponent : public ObjectComponent
{
public:
    CameraComponent(Object& owner);

    void update(FrameInfo& frame_info) override;

    void setPerspectiveProjection(float fovy, float aspect);
    void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

    [[nodiscard]] const glm::mat4& getProjection() const { return projection; }
    [[nodiscard]] const glm::mat4& getView() const { return view; }
    [[nodiscard]] const glm::mat4& getInverseView() const { return inverse_view; }

    static constexpr float CAMERA_NEAR = 0.1f;
    static constexpr float CAMERA_FAR = 100.0f;

private:
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
    glm::mat4 inverse_view{1.f};
};
