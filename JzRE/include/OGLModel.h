#pragma once

#include "CommonTypes.h"
#include "OGLMesh.h"
#include "OGLResourceManager.h"

namespace JzRE {
class OGLModel {
public:
    // model data
    List<OGLMesh> meshes;
    String directory;
    Bool gammaCorrection;

    OGLModel(const String &path, Bool gamma = false);

    void Draw(SharedPtr<OGLShader> shader);

private:
    void LoadModel(const String &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    OGLMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    List<SharedPtr<OGLTexture>> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, String typeName);
};
} // namespace JzRE