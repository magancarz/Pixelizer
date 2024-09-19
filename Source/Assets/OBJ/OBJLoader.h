#pragma once

#include <Assets/MeshData.h>

#include "Assets/AssetManager.h"
#include "Assets/Model/Model.h"
#include "Assets/Model/ModelData.h"
#include "tiny_obj_loader.h"

class OBJLoader
{
public:
    static MeshData loadMeshFromFile(const std::string& mesh_name);

    inline static const char* OBJ_FILE_EXTENSION{".obj"};

private:
    static tinyobj::ObjReader parseFromFile(const std::string& file_path);
    static void handleOBJParserErrorsAndWarnings(const tinyobj::ObjReader& reader);

    static MeshData loadMeshData(const tinyobj::ObjReader& obj_reader, std::string_view mesh_name);

    static std::vector<MaterialData> loadMaterials(const std::vector<tinyobj::material_t>& obj_materials);
    static std::string getMaterialDiffuseTextureName(const tinyobj::material_t& obj_material);
    static std::string getMaterialNormalMapName(const tinyobj::material_t& obj_material);

    static std::vector<ModelData> loadShapes(
        const tinyobj::ObjReader& reader,
        const std::vector<MaterialData>& obj_materials);
    static std::string getShapeRequiredMaterialName(
        const tinyobj::shape_t& shape,
        const std::vector<MaterialData>& materials_data);
    static Vertex extractVertex(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib);
    static glm::vec3 getVertexPosition(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib);
    static glm::vec3 getVertexNormal(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib);
    static glm::vec2 getVertexTextureCoordinates(const tinyobj::index_t& index, const tinyobj::attrib_t& attrib);
    static void calculateTangentSpaceVectors(Vertex& first_vertex, Vertex& second_vertex, Vertex& third_vertex);
    static void addVertexToModelInfo(ModelData& model_data, std::unordered_map<Vertex, uint32_t>& unique_vertices, const Vertex& vertex);
};
