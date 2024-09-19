#include "MeshComponent.h"

#include "Assets/Model/Model.h"
#include "Assets/Material/Material.h"
#include "Assets/Mesh.h"

MeshComponent::MeshComponent(Object& owner)
    : ObjectComponent(owner) {}

void MeshComponent::setMesh(Mesh* in_mesh)
{
    mesh = in_mesh;
    updateModelDescriptions();
    updateMaterials(in_mesh->materials);
}

void MeshComponent::updateModelDescriptions()
{
    model_descriptions.clear();
    for (auto model : mesh->models)
    {
        model_descriptions.emplace_back(model->getModelDescription());
    }
}

void MeshComponent::updateRequiredMaterials()
{
    required_materials.clear();
    for (auto model : mesh->models)
    {
        required_materials.emplace_back(model->getRequiredMaterial());
    }
}

void MeshComponent::updateMaterials(std::vector<Material*> in_materials)
{
    materials = std::move(in_materials);
    updateRequiredMaterials();
}

Material* MeshComponent::findMaterial(const std::string_view& name) const
{
    const auto found_material = std::ranges::find_if(materials.begin(), materials.end(),
        [&] (const Material* material)
    {
        return material->getName() == name;
    });

    return found_material != materials.end() ? *found_material : nullptr;
}

std::vector<std::string> MeshComponent::getRequiredMaterials() const
{
    return required_materials;
}

MeshDescription MeshComponent::getDescription() const
{
    MeshDescription mesh_description{};
    mesh_description.model_descriptions = model_descriptions;
    mesh_description.required_materials = required_materials;
    return mesh_description;
}