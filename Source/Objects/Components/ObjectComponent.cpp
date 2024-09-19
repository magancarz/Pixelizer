#include "ObjectComponent.h"

ObjectComponent::ObjectComponent(Object& owner)
    : owner{owner} {}

ObjectComponent::ObjectComponent(Object& object, const glm::vec3& relative_location)
    : owner{object}, relative_location{relative_location} {}

void ObjectComponent::setRelativeLocation(const glm::vec3& relative_location)
{
    this->relative_location = relative_location;
}
