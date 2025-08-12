#include "JzModel.h"
#include "JzEditorActions.h"
#include "JzRHIDevice.h"
#include "JzServiceContainer.h"

#define JzRE_DEVICE() JzRE::JzServiceContainer::Get<JzRE::JzEditorActions>().GetContext().GetDevice()

namespace JzRE {
JzModel::JzModel(const String &path, Bool gamma) :
    gammaCorrection(gamma)
{
    LoadModel(path);
}

JzModel::~JzModel()
{
    // RHI resources will be automatically cleaned up by shared_ptr
    m_loadedTextures.clear();
}

void JzModel::Draw(std::shared_ptr<JzRHIPipeline> pipeline)
{
    for (auto &mesh : this->meshes) {
        mesh.Draw(pipeline);
    }
}

void JzModel::LoadModel(const String &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene   *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    this->directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void JzModel::ProcessNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for (U32 i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(ProcessMesh(mesh, scene));
    }

    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (U32 i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

JzMesh JzModel::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    std::vector<JzVertex>                      vertices;
    std::vector<U32>                           indices;
    std::vector<std::shared_ptr<JzRHITexture>> textures;

    // walk through each of the mesh's vertices
    for (U32 i = 0; i < mesh->mNumVertices; i++) {
        JzVertex vertex;

        // positions
        vertex.Position = JzVec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // normals
        if (mesh->HasNormals()) {
            vertex.Normal = JzVec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertex.TexCoords = JzVec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            // tangent
            vertex.Tangent = JzVec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            // bitangent
            vertex.Bitangent = JzVec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        } else {
            vertex.TexCoords = JzVec2(0.0f, 0.0f);
        }

        vertices.push_back(std::move(vertex));
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (U32 i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (U32 j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "material.diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "material.specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    auto normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "material.normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    auto heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "material.height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return JzMesh(vertices, indices, textures);
}

std::vector<std::shared_ptr<JzRHITexture>> JzModel::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, String typeName)
{
    std::vector<std::shared_ptr<JzRHITexture>> textures;
    for (U32 i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        String textureName = typeName + "[" + std::to_string(i) + "]";
        String texturePath = this->directory + '/' + str.C_Str();

        auto texture = LoadTexture(texturePath, textureName);
        if (texture) {
            textures.push_back(texture);
        }
    }
    return textures;
}

std::shared_ptr<JzRHITexture> JzModel::LoadTexture(const String &path, const String &typeName)
{
    // Check if texture is already loaded
    auto it = m_loadedTextures.find(path);
    if (it != m_loadedTextures.end()) {
        return it->second;
    }

    auto device = JzRE_DEVICE();
    if (!device) {
        return nullptr;
    }

    // Create texture description
    JzTextureDesc textureDesc{};
    textureDesc.type      = JzETextureType::Texture2D;
    textureDesc.format    = JzETextureFormat::RGBA8;
    textureDesc.minFilter = JzETextureFilter::Linear;
    textureDesc.magFilter = JzETextureFilter::Linear;
    textureDesc.wrapS     = JzETextureWrap::Repeat;
    textureDesc.wrapT     = JzETextureWrap::Repeat;
    textureDesc.debugName = typeName;

    // Note: In a real implementation, you would load image data from file here
    // For now, we create the texture without initial data
    // The specific implementation would depend on your image loading library
    textureDesc.width  = 1; // Placeholder values
    textureDesc.height = 1;
    textureDesc.data   = nullptr; // Would be loaded from file

    auto texture = device->CreateTexture(textureDesc);
    if (texture) {
        m_loadedTextures[path] = texture;
    }

    return texture;
}

} // namespace JzRE