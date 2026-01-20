/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzModel.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

JzRE::JzModel::JzModel(const JzRE::String &path) :
    m_path(path)
{
    // Extract directory path for relative texture/material paths
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == String::npos) {
        lastSlash = path.find_last_of('\\');
    }
    m_directory = (lastSlash != String::npos) ? path.substr(0, lastSlash) : ".";
    m_state     = JzEResourceState::Unloaded;
}

JzRE::JzModel::~JzModel()
{
    Unload();
}

JzRE::Bool JzRE::JzModel::Load()
{
    if (m_state == JzEResourceState::Loaded) return true;
    m_state = JzEResourceState::Loading;

    Assimp::Importer importer;
    const aiScene   *scene = importer.ReadFile(m_path,
                                               aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_FlipWindingOrder);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        // Log error: importer.GetErrorString()
        m_state = JzEResourceState::Error;
        return false;
    }

    // First, process all materials from the MTL file
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial *mat      = scene->mMaterials[i];
        auto        material = ProcessMaterial(mat, scene);
        m_materials.push_back(material);
    }

    // Then process the node hierarchy and meshes
    ProcessNode(scene->mRootNode, scene);

    m_state = JzEResourceState::Loaded;
    return true;
}

void JzRE::JzModel::Unload()
{
    m_nodes.clear();
    m_meshes.clear();
    m_materials.clear();
    m_state = JzEResourceState::Unloaded;
}

void JzRE::JzModel::ProcessNode(aiNode *node, const aiScene *scene)
{
    Node newNode;
    newNode.name = node->mName.C_Str();
    // TODO: Convert Assimp matrix to JzMat4
    // newNode.transform = ConvertAssimpMatrix(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(ProcessMesh(mesh, scene));
        newNode.meshIndices.push_back(static_cast<U32>(m_meshes.size() - 1));
    }

    m_nodes.push_back(newNode);
    uint32_t currentNodeIndex = static_cast<uint32_t>(m_nodes.size() - 1);

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
        m_nodes[currentNodeIndex].childrenIndices.push_back(static_cast<U32>(m_nodes.size() - 1));
    }
}

std::shared_ptr<JzRE::JzMesh> JzRE::JzModel::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<JzVertex> vertices;
    std::vector<uint32_t> indices;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        JzVertex vertex;
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        if (mesh->HasNormals()) {
            vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.TexCoords = {0.0f, 0.0f};
        }

        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent   = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
            vertex.Bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Get material index from the mesh
    I32 materialIndex = static_cast<I32>(mesh->mMaterialIndex);

    // Create mesh with material index
    auto meshResource = std::make_shared<JzMesh>(vertices, indices, materialIndex);
    meshResource->Load(); // Create GPU resources
    return meshResource;
}

std::shared_ptr<JzRE::JzMaterial> JzRE::JzModel::ProcessMaterial(aiMaterial *mat, const aiScene *scene)
{
    JzMaterialProperties props;

    // Get material name
    aiString name;
    if (mat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
        props.name = name.C_Str();
    }

    // Get ambient color (Ka in MTL)
    aiColor3D ambient(0.1f, 0.1f, 0.1f);
    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS) {
        props.ambientColor = JzVec3(ambient.r, ambient.g, ambient.b);
    }

    // Get diffuse color (Kd in MTL)
    aiColor3D diffuse(0.8f, 0.8f, 0.8f);
    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
        props.diffuseColor = JzVec3(diffuse.r, diffuse.g, diffuse.b);
    }

    // Get specular color (Ks in MTL)
    aiColor3D specular(0.5f, 0.5f, 0.5f);
    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
        props.specularColor = JzVec3(specular.r, specular.g, specular.b);
    }

    // Get shininess (Ns in MTL)
    float shininess = 32.0f;
    if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
        props.shininess = shininess;
    }

    // Get opacity (d in MTL)
    float opacity = 1.0f;
    if (mat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
        props.opacity = opacity;
    } else {
        props.opacity = 1.0f;
    }

    // Get diffuse texture path (map_Kd in MTL)
    aiString texturePath;
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
            // Build full path relative to the model directory
            props.diffuseTexturePath = m_directory + "/" + texturePath.C_Str();
        }
    }

    // Create material with properties
    auto material = std::make_shared<JzMaterial>(props);

    // Load and set diffuse texture if available
    if (!props.diffuseTexturePath.empty()) {
        auto texture = std::make_shared<JzTexture>(props.diffuseTexturePath);
        if (texture->Load()) {
            material->SetDiffuseTexture(texture->GetRhiTexture());
        }
    }

    return material;
}
