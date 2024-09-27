#pragma once

#include <vector>

#include "ObjectComponent.h"
#include "Assets/Model/MeshDescription.h"
#include "Assets/Mesh.h"

class Model;
class Material;

class MeshComponent : public ObjectComponent
{
public:
    explicit MeshComponent(Object& owner);

    void setMesh(Mesh* in_mesh);

    [[nodiscard]] Mesh* getMesh() const { return mesh; }
    [[nodiscard]] std::string getMeshName() const { return mesh->name; }
    [[nodiscard]] std::vector<Model*> getModels() const { return mesh->models; }

    [[nodiscard]] std::vector<Material*> getMaterials() const { return materials; }
    [[nodiscard]] Material* findMaterial(const std::string_view& name) const;
    [[nodiscard]] std::vector<std::string> getRequiredMaterials() const;

    [[nodiscard]] MeshDescription getDescription() const;

private:
    void updateMaterials(std::vector<Material*> in_materials);

    Mesh* mesh{nullptr};
    std::vector<Material*> materials{};

    void updateModelDescriptions();

    std::vector<ModelDescription> model_descriptions;

    void updateRequiredMaterials();

    std::vector<std::string> required_materials;
};
