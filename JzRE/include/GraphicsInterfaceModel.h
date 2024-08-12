#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceMesh.h"
#include "GraphicsInterfaceResourceManager.h"

namespace JzRE {
class GraphicsInterfaceModel {
public:
    // model data
    List<GraphicsInterfaceMesh> meshes;
    String directory;
    Bool gammaCorrection;

    GraphicsInterfaceModel(const String &path, Bool gamma = false);

    void Draw(SharedPtr<GraphicsInterfaceShader> shader);

private:
    void LoadModel(const String &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    GraphicsInterfaceMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    List<SharedPtr<GraphicsInterfaceTexture>> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, String typeName);
};
} // namespace JzRE