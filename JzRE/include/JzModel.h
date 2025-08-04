#pragma once

#include "CommonTypes.h"
#include "JzMesh.h"
#include "OGLResourceManager.h"

namespace JzRE {
/**
 * @brief Model class
 */
class JzModel {
public:
    /**
     * @brief Constructor
     *
     * @param path The path to the model
     * @param gamma The gamma correction
     */
    JzModel(const String &path, Bool gamma = false);

    /**
     * @brief Draw the model
     * @param shader The shader to use
     */
    void Draw(std::shared_ptr<OGLShader> shader);

private:
    void                                     LoadModel(const String &path);
    void                                     ProcessNode(aiNode *node, const aiScene *scene);
    JzMesh                                   ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<std::shared_ptr<OGLTexture>> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, String typeName);

public:
    std::vector<JzMesh> meshes;
    String              directory;
    Bool                gammaCorrection;
};
} // namespace JzRE