// #include "gtest/gtest.h"
// #include "gmock/gmock-matchers.h"
//
// #include "Objects/Components/Movement/PlayerMovementComponent.h"
// #include "Objects/Components/Movement/TransformComponent.h"
// #include "Objects/Object.h"
// #include "RenderEngine/FrameInfo.h"
//
// #include "Mocks/MockInputManager.h"
// #include "TestUtils.h"
//
// using testing::Return;
// using testing::AtLeast;
// using testing::_;
//
// struct PlayerMovementComponentTests : public ::testing::Test
// {
//     Object owner;
//
//     void SetUp() override
//     {
//         owner = Object{};
//         owner.addRootComponent(std::make_unique<TransformComponent>(owner));
//     }
// };
//
// TEST_F(PlayerMovementComponentTests, shouldCheckMovementOnEveryAxis)
// {
//     // given
//     auto mock_input_manager = std::make_unique<MockInputManager>();
//     PlayerMovementComponent player_movement_component{owner, *mock_input_manager, owner.findComponentByClass<TransformComponent>()};
//
//     // then
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_LEFT)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_RIGHT)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_FORWARD)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_BACKWARD)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_UP)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_DOWN)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_LEFT)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_RIGHT)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_UP)).Times(1);
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_DOWN)).Times(1);
//
//     FrameInfo frame_info{};
//     float delta_time = 1.f;
//     frame_info.delta_time = delta_time;
//
//     // when
//     player_movement_component.update(frame_info);
// }
//
// TEST_F(PlayerMovementComponentTests, shouldApplyCorrectTranslationWithSomeKeysPressed)
// {
//     // given
//     auto mock_input_manager = std::make_shared<MockInputManager>();
//     auto transform_component = owner.findComponentByClass<TransformComponent>();
//     PlayerMovementComponent player_movement_component{owner, *mock_input_manager, transform_component};
//
//     FrameInfo frame_info{};
//     float delta_time = 1.f;
//     frame_info.delta_time = delta_time;
//
//     glm::vec3 expected_move_dir{-1, 0, -1};
//     glm::vec3 expected_translation
//     {
//         player_movement_component.getMoveSpeed() * delta_time,
//         0,
//         player_movement_component.getMoveSpeed() * delta_time
//     };
//     expected_translation *= glm::normalize(expected_move_dir);
//
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(_)).Times(AtLeast(1));
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_BACKWARD)).WillOnce(Return(true));
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(MOVE_LEFT)).WillOnce(Return(true));
//
//     // when
//     player_movement_component.update(frame_info);
//
//     // then
//     TestUtils::expectTwoVectorsToBeEqual(transform_component->translation, expected_translation);
// }
//
// TEST_F(PlayerMovementComponentTests, shouldApplyCorrectRotationWithSomeKeysPressed)
// {
//     // given
//     auto mock_input_manager = std::make_shared<MockInputManager>();
//     auto transform_component = owner.findComponentByClass<TransformComponent>();
//     PlayerMovementComponent player_movement_component{owner, *mock_input_manager, transform_component};
//
//     FrameInfo frame_info{};
//     float delta_time = .1f;
//     frame_info.delta_time = delta_time;
//
//     glm::vec3 expected_look_dir{1, 1, 0};
//     glm::vec3 expected_rotation
//     {
//             player_movement_component.getLookSpeed() * delta_time,
//             player_movement_component.getLookSpeed() * delta_time,
//             0,
//     };
//     expected_rotation *= glm::normalize(expected_look_dir);
//
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(_)).Times(AtLeast(1));
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_DOWN)).WillOnce(Return(true));
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_RIGHT)).WillOnce(Return(true));
//
//     // when
//     player_movement_component.update(frame_info);
//
//     // then
//     TestUtils::printVector(transform_component->rotation);
//     TestUtils::printVector(expected_rotation);
//     TestUtils::expectTwoVectorsToBeEqual(transform_component->rotation, expected_rotation);
// }
//
// TEST_F(PlayerMovementComponentTests, shouldClampRotationWhenValuesAreTooLarge)
// {
//     // given
//     auto mock_input_manager = std::make_shared<MockInputManager>();
//     auto transform_component = owner.findComponentByClass<TransformComponent>();
//     PlayerMovementComponent player_movement_component{owner, *mock_input_manager, transform_component};
//
//     FrameInfo frame_info{};
//     float delta_time = 5.f;
//     frame_info.delta_time = delta_time;
//
//     constexpr float RADIANS_85_DEGREES = glm::radians(85.f);
//     glm::vec3 expected_look_dir{1, 1, 0};
//     glm::vec3 initial_rotation
//     {
//             player_movement_component.getLookSpeed() * delta_time,
//             player_movement_component.getLookSpeed() * delta_time,
//             0,
//     };
//     initial_rotation *= glm::normalize(expected_look_dir);
//     glm::vec3 expected_rotation = initial_rotation;
//     expected_rotation.x = glm::clamp(expected_rotation.x, -RADIANS_85_DEGREES, RADIANS_85_DEGREES);
//     expected_rotation.y = glm::mod(expected_rotation.y, glm::two_pi<float>());
//
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(_)).Times(AtLeast(1));
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_DOWN)).WillOnce(Return(true));
//     EXPECT_CALL(*mock_input_manager, isKeyPressed(LOOK_RIGHT)).WillOnce(Return(true));
//
//     // when
//     player_movement_component.update(frame_info);
//
//     // then
//     TestUtils::printVector(transform_component->rotation);
//     TestUtils::printVector(expected_rotation);
//     TestUtils::expectTwoVectorsToBeEqual(transform_component->rotation, expected_rotation);
// }