/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzModel.h"

JzRE::JzModel::JzModel(const JzRE::String &path, JzRE::Bool gamma) :
    gammaCorrection(gamma)
{
    // LoadModel(path);
}

JzRE::JzModel::JzModel(std::vector<JzRE::JzMesh> meshes) :
    meshes(std::move(meshes)), gammaCorrection(false)
{
    // No need to load from file for programmatically created models
}

JzRE::JzModel::~JzModel()
{
    // RHI resources will be automatically cleaned up by shared_ptr
    m_loadedTextures.clear();
}

void JzRE::JzModel::Draw(std::shared_ptr<JzRE::JzRHIPipeline> pipeline)
{
}
