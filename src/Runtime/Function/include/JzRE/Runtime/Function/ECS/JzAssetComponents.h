/**
 * @file    JzAssetComponents.h
 * @brief   ECS components for asset-based rendering
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"
#include "JzRE/Runtime/Resource/JzAssetId.h"

namespace JzRE {

// ==================== Asset Reference Components ====================

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
 */
struct JzMaterialAssetComponent {
    JzMaterialHandle materialHandle; ///< Handle to the material asset

    // Cached material properties (populated by AssetLoadingSystem)
    JzVec4 baseColor{1.0f, 1.0f, 1.0f, 1.0f};
    JzVec3 ambientColor{0.1f, 0.1f, 0.1f};
    JzVec3 diffuseColor{0.8f, 0.8f, 0.8f};
    JzVec3 specularColor{0.5f, 0.5f, 0.5f};
    F32    shininess = 32.0f;
    F32    opacity   = 1.0f;
    F32    metallic  = 0.0f;
    F32    roughness = 0.5f;
    Bool   isReady   = false;

    JzMaterialAssetComponent() = default;

    explicit JzMaterialAssetComponent(JzMaterialHandle handle) :
        materialHandle(handle) { }

    /**
     * @brief Check if the component has a valid material reference
     */
    [[nodiscard]] Bool HasMaterial() const
    {
        return materialHandle.IsValid();
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
 * @brief Shader asset reference component
 */
struct JzShaderAssetComponent {
    JzShaderHandle shaderHandle;
    Bool           isReady = false;

    JzShaderAssetComponent() = default;

    explicit JzShaderAssetComponent(JzShaderHandle handle) :
        shaderHandle(handle) { }
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

// ==================== Render Queue Component ====================

/**
 * @brief Render queue classification
 */
enum class JzERenderQueue : U8 {
    Background  = 0,  ///< Background elements (skybox)
    Opaque      = 10, ///< Opaque geometry (default)
    AlphaTest   = 20, ///< Alpha-tested geometry
    Transparent = 30, ///< Transparent geometry (sorted)
    Overlay     = 40  ///< UI overlays
};

/**
 * @brief Component for render queue and layer assignment
 *
 * Used for render sorting and batching.
 */
struct JzRenderQueueComponent {
    JzERenderQueue queue         = JzERenderQueue::Opaque;
    U32            layer         = 0; ///< Render layer for masking
    I32            sortOrder     = 0; ///< Custom sort order within queue
    Bool           castShadow    = true;
    Bool           receiveShadow = true;

    JzRenderQueueComponent() = default;

    JzRenderQueueComponent(JzERenderQueue q, U32 l = 0) :
        queue(q), layer(l) { }
};

// ==================== Instance Rendering ====================

/**
 * @brief Component for instanced rendering
 *
 * When multiple entities share the same mesh and material,
 * they can be batched for instanced rendering.
 */
struct JzInstanceGroupComponent {
    U32 batchId = 0; ///< Batch group identifier

    JzInstanceGroupComponent() = default;

    explicit JzInstanceGroupComponent(U32 id) :
        batchId(id) { }
};

} // namespace JzRE
