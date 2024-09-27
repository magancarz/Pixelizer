#pragma once

#include "glm/vec3.hpp"

struct FrameInfo;
class World;
class Object;

class ObjectComponent
{
public:
    virtual ~ObjectComponent() = default;

    virtual void update(FrameInfo&) {}

    [[nodiscard]] Object& getOwner() const { return owner; }

    virtual void setRelativeLocation(const glm::vec3& relative_location);
    [[nodiscard]] glm::vec3 getRelativeLocation() const { return relative_location; }

protected:
    explicit ObjectComponent(Object& object);
    explicit ObjectComponent(Object& object, const glm::vec3& relative_location);

    Object& owner;
    glm::vec3 relative_location{0};
};
