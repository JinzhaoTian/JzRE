/**
 * @file    JzAssetComponents.h
 * @brief   ECS components for asset-based rendering
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"
#include "JzRE/Runtime/Resource/JzAssetId.h"

namespace JzRE {

// Forward declarations
class JzRHIPipeline;

// ==================== Asset Reference Components ====================

/**
 * @brief Component storing asset file paths for serialization
 *
 * This component stores the original file paths used to load assets.
 * It enables scene serialization by preserving the path information
 * that would otherwise be lost when converting to runtime handles.
 *
 * @note Asset handles are runtime-only (generational IDs). This component
 *       provides the path information needed for save/load operations.
 */
struct JzAssetPathComponent {
    String modelPath;    ///< Path to the model file (e.g., "Content/Models/cube.obj")
    String materialPath; ///< Optional material override path
    String shaderPath;   ///< Optional shader override path

    JzAssetPathComponent() = default;

    explicit JzAssetPathComponent(const String &model) :
        modelPath(model) { }

    JzAssetPathComponent(const String &model, const String &material) :
        modelPath(model), materialPath(material) { }

    /**
     * @brief Check if this component has a valid model path
     */
    [[nodiscard]] Bool HasModelPath() const
    {
        return !modelPath.empty();
    }
};

/**
 * @brief Mesh asset reference component
 *
 * This is a pure data component that references a mesh asset via handle
 * instead of directly holding GPU resources. The actual GPU resources
 * are managed by the JzAssetManager.
 *
 * @note Use this instead of JzMeshComponent for ECS-friendly asset management.
 *       The AssetLoadingSystem will update the cached data once the asset loads.
 */
struct JzMeshAssetComponent {
    JzMeshHandle meshHandle; ///< Handle to the mesh asset

    // Cached render data (populated by AssetLoadingSystem)
    U32  indexCount    = 0;     ///< Number of indices (for draw call)
    I32  materialIndex = -1;    ///< Material slot index
    Bool isReady       = false; ///< Whether the asset is loaded and ready

    JzMeshAssetComponent() = default;

    explicit JzMeshAssetComponent(JzMeshHandle handle) :
        meshHandle(handle) { }

    /**
     * @brief Check if the component has a valid mesh reference
     */
    [[nodiscard]] Bool HasMesh() const
    {
        return meshHandle.IsValid();
    }
};

/**
 * @brief Material asset reference component
 *
 * References a material asset via handle. Caches frequently-accessed
 * material properties for efficient rendering.
 *
 * Enhanced to support shader integration with automatic variant selection
 * based on material features.
 */
struct JzMaterialAssetComponent {
    JzMaterialHandle    materialHandle;        ///< Handle to the material asset
    JzShaderHandle shaderHandle;          ///< Handle to associated shader (optional)
    JzTextureHandle     diffuseTextureHandle;  ///< Handle to diffuse texture (map_Kd)
    JzTextureHandle     normalTextureHandle;   ///< Handle to normal map
    JzTextureHandle     specularTextureHandle; ///< Handle to specular map

    /// Shader keyword bits mapped by cooked shader manifests.
    static constexpr U64 KeywordUseDiffuseMap  = 1ULL << 0;
    static constexpr U64 KeywordUseNormalMap   = 1ULL << 1;
    static constexpr U64 KeywordUseSpecularMap = 1ULL << 2;
    static constexpr U64 KeywordUsePbr         = 1ULL << 3;

    /// Shader variant keyword mask based on material features.
    U64 shaderKeywordMask = KeywordUsePbr;

    /// Cached shader pipeline (populated by AssetLoadingSystem)
    std::shared_ptr<JzRHIPipeline> cachedShaderVariant;

    // Cached material properties (populated by AssetLoadingSystem)
    JzVec4 baseColor{1.0f, 1.0f, 1.0f, 1.0f};
    JzVec3 ambientColor{0.1f, 0.1f, 0.1f};
    JzVec3 diffuseColor{0.8f, 0.8f, 0.8f};
    JzVec3 specularColor{0.5f, 0.5f, 0.5f};
    F32    shininess          = 32.0f;
    F32    opacity            = 1.0f;
    F32    metallic           = 0.0f;
    F32    roughness          = 0.5f;
    Bool   isReady            = false;
    Bool   hasDiffuseTexture  = false; ///< Whether a diffuse texture is bound
    Bool   hasNormalTexture   = false; ///< Whether a normal map is bound
    Bool   hasSpecularTexture = false; ///< Whether a specular map is bound

    JzMaterialAssetComponent() = default;

    explicit JzMaterialAssetComponent(JzMaterialHandle handle) :
        materialHandle(handle) { }

    JzMaterialAssetComponent(JzMaterialHandle matHandle, JzShaderHandle shaderHdl) :
        materialHandle(matHandle), shaderHandle(shaderHdl) { }

    /**
     * @brief Check if the component has a valid material reference
     */
    [[nodiscard]] Bool HasMaterial() const
    {
        return materialHandle.IsValid();
    }

    /**
     * @brief Check if the component has a valid shader reference
     */
    [[nodiscard]] Bool HasShader() const
    {
        return shaderHandle.IsValid();
    }

    /**
     * @brief Check if the component has a valid diffuse texture
     */
    [[nodiscard]] Bool HasDiffuseTexture() const
    {
        return diffuseTextureHandle.IsValid() && hasDiffuseTexture;
    }

    /**
     * @brief Update shader keyword mask based on current material features.
     *
     * Call this after changing texture bindings to ensure the correct
     * cooked shader variant is selected.
     */
    void UpdateShaderKeywordMask()
    {
        shaderKeywordMask = KeywordUsePbr;

        if (hasDiffuseTexture) {
            shaderKeywordMask |= KeywordUseDiffuseMap;
        }
        if (hasNormalTexture) {
            shaderKeywordMask |= KeywordUseNormalMap;
        }
        if (hasSpecularTexture) {
            shaderKeywordMask |= KeywordUseSpecularMap;
        }
    }
};

/**
 * @brief Texture asset reference component
 *
 * References a texture asset via handle.
 */
struct JzTextureAssetComponent {
    JzTextureHandle textureHandle;
    Bool            isReady = false;

    JzTextureAssetComponent() = default;

    explicit JzTextureAssetComponent(JzTextureHandle handle) :
        textureHandle(handle) { }
};

/**
 * @brief Model asset reference component
 *
 * References a model asset (which may contain multiple meshes and materials).
 * Used for batch loading entire model files.
 */
struct JzModelAssetComponent {
    JzModelHandle modelHandle;
    Bool          isReady = false;

    JzModelAssetComponent() = default;

    explicit JzModelAssetComponent(JzModelHandle handle) :
        modelHandle(handle) { }
};

/**
 * @brief Shader reference component
 *
 * Enhanced component supporting shader variants based on defines.
 * The AssetLoadingSystem will automatically compile and cache the
 * appropriate variant based on keywordMask.
 */
struct JzShaderComponent {
    JzShaderHandle shaderHandle;

    /// Shader variant keyword bitmask.
    U64 keywordMask = 0;

    /// Cached compiled pipeline (populated by AssetLoadingSystem)
    std::shared_ptr<JzRHIPipeline> cachedVariant;

    Bool isReady = false;

    JzShaderComponent() = default;

    explicit JzShaderComponent(JzShaderHandle handle) :
        shaderHandle(handle) { }

    JzShaderComponent(JzShaderHandle handle, U64 mask) :
        shaderHandle(handle), keywordMask(mask) { }

    /**
     * @brief Check if the component has a valid shader reference
     */
    [[nodiscard]] Bool HasShader() const
    {
        return shaderHandle.IsValid();
    }

    /**
     * @brief Check if the cached variant is valid and ready to use
     */
    [[nodiscard]] Bool HasValidVariant() const
    {
        return cachedVariant != nullptr && isReady;
    }
};

// ==================== Asset State Tags ====================

/**
 * @brief Tag component marking an entity as waiting for assets to load
 *
 * Added when any asset component on the entity is not yet ready.
 * Removed by AssetLoadingSystem once all assets are loaded.
 */
struct JzAssetLoadingTag { };

/**
 * @brief Tag component marking an entity as having all assets ready
 *
 * Added by AssetLoadingSystem once all asset components are ready.
 * This tag enables efficient filtering in render systems.
 */
struct JzAssetReadyTag { };

/**
 * @brief Tag component marking an entity as having failed asset loads
 *
 * Added when one or more assets fail to load.
 */
struct JzAssetLoadFailedTag { };

// ==================== Hot Reload Dirty Tags ====================

/**
 * @brief Tag component marking an entity as needing shader update
 *
 * Added by JzAssetSystem hot reload when a shader is reloaded.
 * Should be processed by render systems to update GPU state.
 */
struct JzShaderDirtyTag { };

/**
 * @brief Tag component marking an entity as needing texture update
 *
 * Added by JzAssetSystem hot reload when a texture is reloaded.
 * Reserved for future use.
 */
struct JzTextureDirtyTag { };

/**
 * @brief Tag component marking an entity as needing material update
 *
 * Added by JzAssetSystem hot reload when a material is reloaded.
 * Reserved for future use.
 */
struct JzMaterialDirtyTag { };

// ==================== Asset Lifecycle Component ====================

/**
 * @brief Component for tracking all asset references on an entity
 *
 * When this component is destroyed (entity destruction), it should
 * release references to all tracked assets. This enables automatic
 * cleanup and memory management.
 *
 * @note This component is optional but recommended for proper
 *       resource lifecycle management.
 */
struct JzAssetReferenceComponent {
    std::vector<JzAssetId> meshRefs;     ///< Referenced mesh asset IDs
    std::vector<JzAssetId> materialRefs; ///< Referenced material asset IDs
    std::vector<JzAssetId> textureRefs;  ///< Referenced texture asset IDs
    std::vector<JzAssetId> modelRefs;    ///< Referenced model asset IDs
    std::vector<JzAssetId> shaderRefs;   ///< Referenced shader asset IDs

    /**
     * @brief Add a mesh reference
     */
    void AddMesh(JzMeshHandle handle)
    {
        if (handle.IsValid()) {
            meshRefs.push_back(handle.GetId());
        }
    }

    /**
     * @brief Add a material reference
     */
    void AddMaterial(JzMaterialHandle handle)
    {
        if (handle.IsValid()) {
            materialRefs.push_back(handle.GetId());
        }
    }

    /**
     * @brief Add a texture reference
     */
    void AddTexture(JzTextureHandle handle)
    {
        if (handle.IsValid()) {
            textureRefs.push_back(handle.GetId());
        }
    }

    /**
     * @brief Add a model reference
     */
    void AddModel(JzModelHandle handle)
    {
        if (handle.IsValid()) {
            modelRefs.push_back(handle.GetId());
        }
    }

    /**
     * @brief Clear all references
     */
    void Clear()
    {
        meshRefs.clear();
        materialRefs.clear();
        textureRefs.clear();
        modelRefs.clear();
        shaderRefs.clear();
    }

    /**
     * @brief Get total number of references
     */
    [[nodiscard]] Size GetTotalRefCount() const
    {
        return meshRefs.size() + materialRefs.size() + textureRefs.size() + modelRefs.size() + shaderRefs.size();
    }
};
} // namespace JzRE
