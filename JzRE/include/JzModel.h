#pragma once

#include "CommonTypes.h"
#include "JzMesh.h"
#include "OGLResourceManager.h"

namespace JzRE {
class JzModel {
public:
    // model data
    std::vector<JzMesh> meshes;
    String              directory;
    Bool                gammaCorrection;

    JzModel(const String &path, Bool gamma = false);

    void Draw(std::shared_ptr<OGLShader> shader);

private:
    void                                     LoadModel(const String &path);
    void                                     ProcessNode(aiNode *node, const aiScene *scene);
    JzMesh                                   ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<std::shared_ptr<OGLTexture>> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, String typeName);
};
} // namespace JzRE