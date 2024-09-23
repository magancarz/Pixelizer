#include "OBJLoader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>

#include "Logs/LogSystem.h"
#include "Utils/PathBuilder.h"
#include "Utils/Algorithms.h"
#include "Assets/Model/Vertex.h"
#include "Assets/Model/ModelData.h"
#include "Assets/MeshData.h"

namespace std
{
    template <>
    struct hash<Vertex> {
        size_t operator()(Vertex const &vertex) const noexcept
        {
            size_t seed = 0;
            Algorithms::hashCombine(seed, vertex.position, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

MeshData OBJLoader::loadMeshFromFile(const std::string& mesh_name)
{
    const std::string filepath = PathBuilder().append(Assets::MODELS_DIRECTORY_PATH.string()).append(mesh_name).fileExtension(OBJ_FILE_EXTENSION).build();
    tinyobj::ObjReader obj_reader = parseFromFile(filepath);

    if (!obj_reader.Valid())
    {
        return MeshData{};
    }

    return loadMeshData(obj_reader, mesh_name);
}

tinyobj::ObjReader OBJLoader::parseFromFile(const std::string& file_path)
{
    tinyobj::ObjReader obj_reader;
    tinyobj::ObjReaderConfig reader_config;
    obj_reader.ParseFromFile(file_path, reader_config);
    handleOBJParserErrorsAndWarnings(obj_reader);

    return obj_reader;
}

void OBJLoader::handleOBJParserErrorsAndWarnings(const tinyobj::ObjReader& reader)
{
    if (!reader.Valid())
    {
        LogSystem::log(LogSeverity::ERROR, "TinyObjReader: ", reader.Error());
    }

    if (!reader.Warning().empty())
    {
        LogSystem::log(LogSeverity::WARNING, "TinyObjReader: ", reader.Warning());
    }
}

MeshData OBJLoader::loadMeshData(const tinyobj::ObjReader& obj_reader, std::string_view mesh_name)
{
    MeshData mesh_data{};
    mesh_data.name = mesh_name;
    mesh_data.materials_data = loadMaterials(obj_reader.GetMaterials());
    mesh_data.models_data = loadShapes(obj_reader, mesh_data.materials_data);

    return mesh_data;
}

std::vector<MaterialData> OBJLoader::loadMaterials(const std::vector<tinyobj::material_t>& obj_materials)
{
    if (obj_materials.empty())
    {
        return { MaterialData{} };
    }

    std::vector<MaterialData> materials_data;
    materials_data.reserve(obj_materials.size());
    for (auto& obj_material : obj_materials)
    {
        MaterialData material_data{};
        material_data.name = obj_material.name;
        material_data.diffuse_texture_name = getMaterialDiffuseTextureName(obj_material);
        materials_data.emplace_back(material_data);
    }

    return materials_data;
}

std::string OBJLoader::getMaterialDiffuseTextureName(const tinyobj::material_t& obj_material)
{
    if (obj_material.diffuse_texname.empty())
    {
        return Assets::DEFAULT_DIFFUSE_TEXTURE;
    }

    return obj_material.diffuse_texname;
}

std::string OBJLoader::getMaterialNormalMapName(const tinyobj::material_t& obj_material)
{
    if (obj_material.displacement_texname.empty())
    {
        return Assets::DEFAULT_NORMAL_MAP;
    }

    return obj_material.displacement_texname;
}

std::vector<ModelData> OBJLoader::loadShapes(
        const tinyobj::ObjReader& reader,
        const std::vector<MaterialData>& materials_data)
{
    const tinyobj::attrib_t& attrib = reader.GetAttrib();

    std::vector<tinyobj::shape_t> shapes = reader.GetShapes();
    std::vector<ModelData> models_data;
    models_data.reserve(shapes.size());

    for (const auto& shape : shapes)
    {
        ModelData model_data{};
        model_data.name = shape.name;
        model_data.required_material = getShapeRequiredMaterialName(shape, materials_data);

        std::unordered_map<Vertex, uint32_t> unique_vertices{};
        for (size_t i = 0; i < shape.mesh.indices.size(); i += 3)
        {
            Vertex first_vertex = extractVertex(shape.mesh.indices[i + 0], attrib);
            Vertex second_vertex = extractVertex(shape.mesh.indices[i + 1], attrib);
            Vertex third_vertex = extractVertex(shape.mesh.indices[i + 2], attrib);

            addVertexToModelInfo(model_data, unique_vertices, first_vertex);
            addVertexToModelInfo(model_data, unique_vertices, second_vertex);
            addVertexToModelInfo(model_data, unique_vertices, third_vertex);
        }

        models_data.emplace_back(model_data);
    }

    return models_data;
}

std::string OBJLoader::getShapeRequiredMaterialName(const tinyobj::shape_t& shape, const std::vector<MaterialData>& materials_data)
{
    if (int material_id = shape.mesh.material_ids[0]; material_id >= 0 && !materials_data.empty())
    {
        return materials_data[material_id].name;
    }

    return Assets::DEFAULT_MATERIAL_NAME;
}

Vertex OBJLoader::extractVertex(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib)
{
    Vertex vertex{};
    vertex.position = getVertexPosition(index, attrib);
    vertex.normal = getVertexNormal(index, attrib);
    vertex.uv = getVertexTextureCoordinates(index, attrib);

    return vertex;
}

glm::vec3 OBJLoader::getVertexPosition(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib)
{
    glm::vec3 position{0, 0, 0};

    if (index.vertex_index >= 0)
    {
        position =
        {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };
    }

    return position;
}

glm::vec3 OBJLoader::getVertexNormal(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib)
{
    glm::vec3 normal{0, 0, 1};
    if (index.normal_index >= 0)
    {
        normal =
        {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
        };
    }

    return normal;
}

glm::vec2 OBJLoader::getVertexTextureCoordinates(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib)
{
    glm::vec2 uv{0, 0};
    if (index.texcoord_index >= 0)
    {
        uv =
        {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0 - attrib.texcoords[2 * index.texcoord_index + 1],
        };
    }

    return uv;
}

void OBJLoader::addVertexToModelInfo(ModelData& model_data, std::unordered_map<Vertex, uint32_t>& unique_vertices, const Vertex& vertex)
{
    if (!unique_vertices.contains(vertex))
    {
        unique_vertices[vertex] = static_cast<uint32_t>(model_data.vertices.size());
        model_data.vertices.push_back(vertex);
    }
    model_data.indices.push_back(unique_vertices[vertex]);
}