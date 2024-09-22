#include "ViewerMovementComponent.h"

#include "RenderEngine/FrameInfo.h"
#include "Input/InputManager.h"
#include "TransformComponent.h"
#include "Input/KeyCodes.h"
#include "Logs/LogSystem.h"

ViewerMovementComponent::ViewerMovementComponent(Object& owner, InputManager& input_manager, TransformComponent& transform_component)
    : ObjectComponent(owner), input_manager{input_manager}, transform_component{transform_component} {}

void ViewerMovementComponent::update(FrameInfo& frame_info)
{
    movePlayerWithInput(frame_info);
    rotatePlayerWithInput(frame_info);
}

void ViewerMovementComponent::movePlayerWithInput(FrameInfo& frame_info) const
{
    glm::vec3 rotation_in_euler_angles = transform_component.getRotationInEulerAngles();
    const float yaw = rotation_in_euler_angles.y;
    const glm::vec3 forward_dir{glm::sin(yaw), 0.f, glm::cos(yaw)};
    const glm::vec3 right_dir{forward_dir.z, 0.f, -forward_dir.x};
    constexpr glm::vec3 up_dir{0.f, 1.f, 0.f};

    glm::vec3 translation_input{0.f};
    translation_input += forward_dir * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_FORWARD));
    translation_input -= forward_dir * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_BACKWARD));
    translation_input += right_dir * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_RIGHT));
    translation_input -= right_dir * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_LEFT));
    translation_input += up_dir * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_UP));
    translation_input -= up_dir * static_cast<float>(input_manager.isKeyPressed(KeyCode::MOVE_DOWN));
    translation_input += 12.f * up_dir * static_cast<float>(input_manager.isKeyPressed(KeyCode::JUMP));

    if (glm::dot(translation_input, translation_input) > std::numeric_limits<float>::epsilon())
    {
        translation_input = move_speed * frame_info.delta_time * glm::normalize(translation_input);
        transform_component.translate(translation_input);
    }
}

void ViewerMovementComponent::rotatePlayerWithInput(FrameInfo& frame_info) const
{
    glm::vec3 rotation_input{0.0f};

    rotation_input.x += 1.0f * static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_DOWN));
    rotation_input.x -= 1.0f * static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_UP));
    rotation_input.y += 1.0f * static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_RIGHT));
    rotation_input.y -= 1.0f * static_cast<float>(input_manager.isKeyPressed(KeyCode::LOOK_LEFT));

    if (glm::dot(rotation_input, rotation_input) > std::numeric_limits<float>::epsilon())
    {
        transform_component.rotateWithEulerAngles(look_speed * frame_info.delta_time * rotation_input);
    }

    glm::vec3 rotation_in_euler_angles = transform_component.getRotationInEulerAngles();
    constexpr float X_ROTATION_LIMIT = glm::radians(85.f);
    transform_component.setRotationInEulerAngles(
        glm::vec3
        {
            glm::clamp(rotation_in_euler_angles.x, -X_ROTATION_LIMIT, X_ROTATION_LIMIT),
            glm::mod(rotation_in_euler_angles.y, glm::two_pi<float>()),
            rotation_in_euler_angles.z
        });
}
