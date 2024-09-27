#pragma once

#include <memory>
#include <vector>

#include "Components/ObjectComponent.h"

struct FrameInfo;

class Object
{
public:
    using id_t = uint32_t;

    Object();
    virtual ~Object() = default;

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) = default;
    Object& operator=(Object&&) = default;

    [[nodiscard]] id_t getID() const { return id; }

    virtual void update(FrameInfo& frame_info);

    void addComponent(std::unique_ptr<ObjectComponent> component);

    template <typename T>
    T* findComponentByClass()
    {
        for (const auto& component : components)
        {
            if (auto matching_class_component = dynamic_cast<T*>(component.get()))
            {
                return matching_class_component;
            }
        }

        return nullptr;
    }

private:
    explicit Object(id_t object_id) : id{object_id} {}

    inline static id_t available_id = 0;
    id_t id;

    std::vector<std::unique_ptr<ObjectComponent>> components;
};
