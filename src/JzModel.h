/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "JzRETypes.h"
#include "JzMesh.h"
#include "JzRHIPipeline.h"
#include "JzRHITexture.h"

namespace JzRE {
/**
 * @brief Model class - Platform-independent model using RHI
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
     * @brief Constructor for programmatically created models
     *
     * @param meshes The meshes to add to the model
     */
    JzModel(std::vector<JzMesh> meshes);

    /**
     * @brief Destructor
     */
    ~JzModel();

    /**
     * @brief Draw the model using RHI
     *
     * @param pipeline The pipeline to use for rendering
     */
    void Draw(std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Get all meshes in this model
     *
     * @return Reference to mesh vector
     */
    const std::vector<JzMesh> &GetMeshes() const
    {
        return meshes;
    }

    /**
     * @brief Get the directory path of the model
     *
     * @return The directory path
     */
    const String &GetDirectory() const
    {
        return directory;
    }

    /**
     * @brief Check if gamma correction is enabled
     *
     * @return True if gamma correction is enabled
     */
    Bool IsGammaCorrectionEnabled() const
    {
        return gammaCorrection;
    }

public:
    std::vector<JzMesh> meshes;
    String              directory;
    Bool                gammaCorrection;

private:
    // Cache for loaded textures to avoid duplicates
    std::map<String, std::shared_ptr<JzRHITexture>> m_loadedTextures;
};
} // namespace JzRE