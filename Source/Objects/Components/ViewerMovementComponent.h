#pragma once

#include <glm/vec3.hpp>

#include "TransformComponent.h"

class InputManager;

class ViewerMovementComponent : public ObjectComponent
{
public:
    ViewerMovementComponent(Object& owner, InputManager& input_manager, TransformComponent& transform_component);

    void update(FrameInfo& frame_info) override;

    [[nodiscard]] float getMoveSpeed() const { return move_speed; }
    [[nodiscard]] float getLookSpeed() const { return look_speed; }

private:
    InputManager& input_manager;
    TransformComponent& transform_component;

    void rotatePlayerWithInput(FrameInfo& frame_info) const;

    float look_speed{0.8f};

    void movePlayerWithInput(FrameInfo& frame_info) const;

    float move_speed{36.f};
};
