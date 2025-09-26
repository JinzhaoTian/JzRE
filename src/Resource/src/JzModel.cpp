/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Resource/JzModel.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

JzRE::JzModel::JzModel(const JzRE::String &path) :
    m_path(path)
{
    m_directory = path.substr(0, path.find_last_of('/'));
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
    const aiScene   *scene = importer.ReadFile(m_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        // In a real engine, log: importer.GetErrorString()
        m_state = JzEResourceState::Error;
        return false;
    }

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
    // Assimp matrices need to be converted to JzMat4
    // newNode.transform = ConvertAssimpMatrix(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(ProcessMesh(mesh, scene));
        newNode.meshIndices.push_back(m_meshes.size() - 1);
    }

    m_nodes.push_back(newNode);
    uint32_t currentNodeIndex = m_nodes.size() - 1;

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
        m_nodes[currentNodeIndex].childrenIndices.push_back(m_nodes.size() - 1);
    }
}

std::shared_ptr<JzRE::JzMesh> JzRE::JzModel::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<JzVertex> vertices;
    std::vector<uint32_t> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        JzVertex vertex;
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        if (mesh->HasNormals()) {
            vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent   = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
            vertex.Bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
        }
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // In a real implementation, you would also process materials here.
    // aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    auto meshResource = std::make_shared<JzMesh>(vertices, indices);
    meshResource->Load(); // Create GPU resources
    return meshResource;
}

// ProcessMaterial and texture loading would be implemented here.
