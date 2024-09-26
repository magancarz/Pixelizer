#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "Objects/Object.h"
#include "Objects/Components/MeshComponent.h"
#include "Objects/Components/TransformComponent.h"
#include "RenderEngine/FrameInfo.h"
#include "Mocks/MockObjectComponent.h"

#include "TestUtils.h"

using ::testing::_;

TEST(ObjectTests, shouldAssignUniqueIDToEachObject)
{
    // given & when
    const Object first_object;

    uint32_t second_object_id{0};
    {
        const Object second_object;
        second_object_id = second_object.getID();
    }

    const Object third_object;

    // then
    EXPECT_TRUE(first_object.getID() != second_object_id);
    EXPECT_TRUE(first_object.getID() != third_object.getID());
    EXPECT_TRUE(second_object_id != third_object.getID());
}

TEST(ObjectTests, shouldAddComponentToObject)
{
    // given
    Object object{};

    auto transform_component = std::make_unique<TransformComponent>(object);

    // when
    object.addComponent(std::move(transform_component));

    // then
    EXPECT_TRUE(object.findComponentByClass<TransformComponent>() != nullptr);
}

TEST(ObjectTests, shouldRootComponentToObject)
{
    // given
    Object object{};

    auto transform_component = std::make_unique<TransformComponent>(object);

    // when
    object.addComponent(std::move(transform_component));

    // then
    EXPECT_TRUE(object.findComponentByClass<TransformComponent>());
}

TEST(ObjectTests, shouldUpdateObjectComponents)
{
    // given
    Object object{};
    auto first_component = std::make_unique<MockObjectComponent>(object);
    MockObjectComponent& first_component_ref = *first_component;
    object.addComponent(std::move(first_component));
    auto second_component = std::make_unique<MockObjectComponent>(object);
    MockObjectComponent& second_component_ref = *second_component;
    object.addComponent(std::move(second_component));

    FrameInfo frame_info{};

    // then
    EXPECT_CALL(first_component_ref, update(_)).Times(1);
    EXPECT_CALL(second_component_ref, update(_)).Times(1);

    // when
    object.update(frame_info);
}

TEST(ObjectTests, shouldNotFindObjectComponent)
{
    // given
    Object object{};

    auto transform_component = std::make_unique<TransformComponent>(object);
    object.addComponent(std::move(transform_component));

    // when
    auto found_component = object.findComponentByClass<MeshComponent>();

    // then
    EXPECT_TRUE(found_component == nullptr);
}

TEST(ObjectTests, shouldFindObjectComponent)
{
    // given
    Object object{};

    auto transform_component = std::make_unique<TransformComponent>(object);
    object.addComponent(std::move(transform_component));

    // when
    auto found_component = object.findComponentByClass<TransformComponent>();

    // then
    EXPECT_TRUE(found_component != nullptr);
}
