/**
 * @file    JzShaderVariantManager.h
 * @brief   Centralized shader variant caching and management
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"

namespace JzRE {

// Forward declarations
class JzShaderAsset;
class JzShaderVariant;
class JzShaderRegistry;
class JzAssetManager;

/**
 * @brief Render features flags for automatic variant selection
 */
struct JzRenderFeatures {
    Bool shadowMapping   = false; ///< Enable shadow mapping
    Bool ssao            = false; ///< Enable screen-space ambient occlusion
    Bool reflections     = false; ///< Enable reflections
    Bool instancing      = false; ///< Enable GPU instancing
    Bool normalMapping   = false; ///< Enable normal mapping
    Bool specularMapping = false; ///< Enable specular mapping
    Bool pbrLighting     = true;  ///< Enable PBR lighting model
    Bool skinning        = false; ///< Enable skeletal skinning

    /**
     * @brief Convert render features to shader defines map
     */
    std::unordered_map<String, String> ToDefines() const
    {
        std::unordered_map<String, String> defines;
        defines["USE_SHADOW_MAP"]   = shadowMapping ? "1" : "0";
        defines["USE_SSAO"]         = ssao ? "1" : "0";
        defines["USE_REFLECTIONS"]  = reflections ? "1" : "0";
        defines["USE_INSTANCING"]   = instancing ? "1" : "0";
        defines["USE_NORMAL_MAP"]   = normalMapping ? "1" : "0";
        defines["USE_SPECULAR_MAP"] = specularMapping ? "1" : "0";
        defines["USE_PBR"]          = pbrLighting ? "1" : "0";
        defines["USE_SKINNING"]     = skinning ? "1" : "0";
        return defines;
    }
};

/**
 * @brief Variant cache key for efficient lookup
 */
struct JzVariantCacheKey {
    JzShaderAssetHandle                baseShader;
    std::unordered_map<String, String> defines;

    Bool operator==(const JzVariantCacheKey &other) const
    {
        return baseShader == other.baseShader && defines == other.defines;
    }

    /**
     * @brief Compute hash for this cache key
     */
    Size Hash() const
    {
        // Use JzAssetHandle's built-in hash
        Size h = JzAssetHandle<JzShaderAsset>::Hash{}(baseShader);
        for (const auto &[key, value] : defines) {
            h ^= std::hash<String>{}(key) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<String>{}(value) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

/**
 * @brief Hash functor for JzVariantCacheKey
 */
struct JzVariantCacheKeyHash {
    Size operator()(const JzVariantCacheKey &key) const
    {
        return key.Hash();
    }
};

/**
 * @brief Centralized shader variant manager
 *
 * This class provides:
 * - Global variant caching to avoid duplicate compilation
 * - Automatic variant selection based on render features
 * - Precompilation of common variant combinations
 * - Cleanup of unused variants
 *
 * @note Use this class for efficient variant management across
 *       multiple materials that share the same base shader.
 */
class JzShaderVariantManager {
public:
    /**
     * @brief Construct variant manager
     * @param registry Reference to the shader registry for compilation
     * @param assetManager Reference to the asset manager for shader access
     */
    JzShaderVariantManager(JzShaderRegistry &registry, JzAssetManager &assetManager);

    /**
     * @brief Destructor
     */
    ~JzShaderVariantManager();

    // Non-copyable
    JzShaderVariantManager(const JzShaderVariantManager &)            = delete;
    JzShaderVariantManager &operator=(const JzShaderVariantManager &) = delete;

    // ==================== Variant Access ====================

    /**
     * @brief Get or compile a variant for the specified shader and defines
     *
     * @param shaderHandle Handle to the base shader
     * @param defines Preprocessor defines for this variant
     * @return Shared pointer to the variant, or nullptr on failure
     */
    std::shared_ptr<JzShaderVariant> GetVariant(
        JzShaderAssetHandle                       shaderHandle,
        const std::unordered_map<String, String> &defines);

    /**
     * @brief Get or compile a variant based on render features
     *
     * Automatically generates appropriate defines from the feature set.
     *
     * @param shaderHandle Handle to the base shader
     * @param features Render features to enable
     * @return Shared pointer to the variant, or nullptr on failure
     */
    std::shared_ptr<JzShaderVariant> GetVariantForFeatures(
        JzShaderAssetHandle     shaderHandle,
        const JzRenderFeatures &features);

    // ==================== Precompilation ====================

    /**
     * @brief Precompile common variants for a shader
     *
     * Generates and compiles all combinations of the specified keywords.
     *
     * @param shaderHandle Handle to the base shader
     * @param featureKeywords List of feature keywords to generate combinations
     */
    void PrecompileCommonVariants(JzShaderAssetHandle        shaderHandle,
                                  const std::vector<String> &featureKeywords);

    // ==================== Cache Management ====================

    /**
     * @brief Clear expired variants from cache
     *
     * Removes variants that are no longer referenced by any material.
     *
     * @return Number of variants removed
     */
    Size CleanupUnusedVariants();

    /**
     * @brief Clear all cached variants
     */
    void ClearCache();

    /**
     * @brief Invalidate all variants for a specific shader
     *
     * Call this when a shader is reloaded to force recompilation.
     *
     * @param shaderHandle Handle to the shader
     */
    void InvalidateShaderVariants(JzShaderAssetHandle shaderHandle);

    // ==================== Statistics ====================

    /**
     * @brief Get number of cached variants
     */
    [[nodiscard]] Size GetCachedVariantCount() const;

    /**
     * @brief Get number of active (referenced) variants
     */
    [[nodiscard]] Size GetActiveVariantCount() const;

    /**
     * @brief Get cache hit rate (0.0 - 1.0)
     */
    [[nodiscard]] F32 GetCacheHitRate() const;

    /**
     * @brief Reset statistics counters
     */
    void ResetStatistics();

private:
    JzShaderRegistry &m_registry;
    JzAssetManager   &m_assetManager;

    // Variant cache: key -> weak_ptr to variant
    std::unordered_map<JzVariantCacheKey, std::weak_ptr<JzShaderVariant>,
                       JzVariantCacheKeyHash>
        m_variantCache;

    // Statistics
    mutable Size m_cacheHits   = 0;
    mutable Size m_cacheMisses = 0;

    mutable std::mutex m_mutex;
};

} // namespace JzRE
