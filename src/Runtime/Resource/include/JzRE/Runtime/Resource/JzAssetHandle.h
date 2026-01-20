/**
 * @file    JzAssetHandle.h
 * @brief   Type-safe asset handle template
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Resource/JzAssetId.h"

namespace JzRE {

// Forward declarations for asset types
class JzMesh;
class JzTexture;
class JzMaterial;
class JzModel;
class JzShader;
class JzShaderAsset;
class JzFont;

/**
 * @brief Type-safe handle to an asset
 *
 * @tparam T The asset type (JzMesh, JzTexture, JzMaterial, etc.)
 *
 * This handle provides type safety at compile time while using the same
 * underlying JzAssetId structure. Handles are lightweight (8 bytes) and
 * can be freely copied.
 *
 * @note Handles do not own the asset. They are merely references that can
 *       be validated through the asset registry's generation check.
 *
 * @example
 * @code
 * JzMeshHandle meshHandle = assetManager.LoadSync<JzMesh>("cube.obj");
 * if (meshHandle.IsValid()) {
 *     JzMesh* mesh = assetManager.Get(meshHandle);
 *     // Use mesh...
 * }
 * @endcode
 */
template <typename T>
class JzAssetHandle {
public:
    /**
     * @brief Default constructor - creates an invalid handle
     */
    JzAssetHandle() = default;

    /**
     * @brief Construct from an asset ID
     */
    explicit JzAssetHandle(JzAssetId id) :
        m_id(id) { }

    /**
     * @brief Check if the handle is valid
     *
     * @note This only checks if the ID is not the invalid sentinel.
     *       To check if the asset still exists and hasn't been replaced,
     *       use JzAssetRegistry::IsValid() or JzAssetManager::IsValid().
     */
    [[nodiscard]] Bool IsValid() const
    {
        return m_id.IsValid();
    }

    /**
     * @brief Get the underlying asset ID
     */
    [[nodiscard]] JzAssetId GetId() const
    {
        return m_id;
    }

    /**
     * @brief Explicit conversion to bool
     */
    explicit operator Bool() const
    {
        return IsValid();
    }

    /**
     * @brief Equality comparison
     */
    Bool operator==(const JzAssetHandle &other) const
    {
        return m_id == other.m_id;
    }

    /**
     * @brief Inequality comparison
     */
    Bool operator!=(const JzAssetHandle &other) const
    {
        return m_id != other.m_id;
    }

    /**
     * @brief Create an invalid handle
     */
    static JzAssetHandle Invalid()
    {
        return JzAssetHandle(JzAssetId::Invalid());
    }

    /**
     * @brief Hash functor for use with unordered containers
     */
    struct Hash {
        Size operator()(const JzAssetHandle &handle) const
        {
            return JzAssetId::Hash{}(handle.m_id);
        }
    };

private:
    JzAssetId m_id = JzAssetId::Invalid();
};

// ==================== Common Handle Type Aliases ====================

using JzMeshHandle        = JzAssetHandle<JzMesh>;
using JzTextureHandle     = JzAssetHandle<JzTexture>;
using JzMaterialHandle    = JzAssetHandle<JzMaterial>;
using JzModelHandle       = JzAssetHandle<JzModel>;
using JzShaderHandle      = JzAssetHandle<JzShader>;
using JzShaderAssetHandle = JzAssetHandle<JzShaderAsset>;
using JzFontHandle        = JzAssetHandle<JzFont>;

} // namespace JzRE
