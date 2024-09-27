#include "Object.h"

#include "Objects/Components/MeshComponent.h"

Object::Object()
    : id{available_id++} {}

void Object::update(FrameInfo& frame_info)
{
    for (auto& component : components)
    {
        component->update(frame_info);
    }
}

void Object::addComponent(std::unique_ptr<ObjectComponent> component)
{
    assert(component->getOwner().getID() == this->getID() && "Component's owner should be the same as the object to which component tried to be added");
    components.emplace_back(std::move(component));
}
