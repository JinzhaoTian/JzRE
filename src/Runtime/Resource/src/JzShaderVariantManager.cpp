/**
 * @file    JzShaderVariantManager.cpp
 * @brief   Implementation of JzShaderVariantManager
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderVariantManager.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"
#include "JzRE/Runtime/Resource/JzShaderRegistry.h"
#include "JzRE/Runtime/Resource/JzShaderVariant.h"

namespace JzRE {

JzShaderVariantManager::JzShaderVariantManager(JzShaderRegistry &registry,
                                               JzAssetManager   &assetManager) :
    m_registry(registry),
    m_assetManager(assetManager)
{
    JzRE_LOG_INFO("JzShaderVariantManager: Initialized");
}

JzShaderVariantManager::~JzShaderVariantManager()
{
    JzRE_LOG_INFO("JzShaderVariantManager: Shutdown with {} cached variants",
                  m_variantCache.size());
}

std::shared_ptr<JzShaderVariant> JzShaderVariantManager::GetVariant(
    JzShaderAssetHandle                       shaderHandle,
    const std::unordered_map<String, String> &defines)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Build cache key
    JzVariantCacheKey key{shaderHandle, defines};

    // Check cache
    auto it = m_variantCache.find(key);
    if (it != m_variantCache.end()) {
        if (auto variant = it->second.lock()) {
            ++m_cacheHits;
            return variant;
        }
        // Variant was expired, remove from cache
        m_variantCache.erase(it);
    }

    ++m_cacheMisses;

    // Get shader asset
    JzShaderAsset *shaderAsset = m_assetManager.Get(shaderHandle);
    if (!shaderAsset) {
        JzRE_LOG_WARN("JzShaderVariantManager: Shader asset not found");
        return nullptr;
    }

    // Compile new variant using registry
    auto variant = m_registry.CompileVariant(shaderAsset, defines);
    if (!variant) {
        JzRE_LOG_WARN("JzShaderVariantManager: Failed to compile variant");
        return nullptr;
    }

    // Cache the variant
    m_variantCache[key] = variant;

    JzRE_LOG_DEBUG("JzShaderVariantManager: Cached new variant for '{}' ({} defines)",
                   shaderAsset->GetName(), defines.size());

    return variant;
}

std::shared_ptr<JzShaderVariant> JzShaderVariantManager::GetVariantForFeatures(
    JzShaderAssetHandle     shaderHandle,
    const JzRenderFeatures &features)
{
    return GetVariant(shaderHandle, features.ToDefines());
}

void JzShaderVariantManager::PrecompileCommonVariants(JzShaderAssetHandle        shaderHandle,
                                                      const std::vector<String> &featureKeywords)
{
    JzShaderAsset *shaderAsset = m_assetManager.Get(shaderHandle);
    if (!shaderAsset) {
        JzRE_LOG_WARN("JzShaderVariantManager: Cannot precompile - shader not found");
        return;
    }

    m_registry.PrecompileCommonVariants(shaderAsset, featureKeywords);

    JzRE_LOG_INFO("JzShaderVariantManager: Precompiled variants for '{}' with {} keywords",
                  shaderAsset->GetName(), featureKeywords.size());
}

Size JzShaderVariantManager::CleanupUnusedVariants()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Size removedCount = 0;

    for (auto it = m_variantCache.begin(); it != m_variantCache.end();) {
        if (it->second.expired()) {
            it = m_variantCache.erase(it);
            ++removedCount;
        } else {
            ++it;
        }
    }

    if (removedCount > 0) {
        JzRE_LOG_DEBUG("JzShaderVariantManager: Cleaned up {} expired variants", removedCount);
    }

    return removedCount;
}

void JzShaderVariantManager::ClearCache()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Size count = m_variantCache.size();
    m_variantCache.clear();

    JzRE_LOG_INFO("JzShaderVariantManager: Cleared {} cached variants", count);
}

void JzShaderVariantManager::InvalidateShaderVariants(JzShaderAssetHandle shaderHandle)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Size removedCount = 0;

    for (auto it = m_variantCache.begin(); it != m_variantCache.end();) {
        if (it->first.baseShader == shaderHandle) {
            it = m_variantCache.erase(it);
            ++removedCount;
        } else {
            ++it;
        }
    }

    if (removedCount > 0) {
        JzRE_LOG_DEBUG("JzShaderVariantManager: Invalidated {} variants for shader", removedCount);
    }
}

Size JzShaderVariantManager::GetCachedVariantCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_variantCache.size();
}

Size JzShaderVariantManager::GetActiveVariantCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Size activeCount = 0;
    for (const auto &[key, weakVariant] : m_variantCache) {
        if (!weakVariant.expired()) {
            ++activeCount;
        }
    }
    return activeCount;
}

F32 JzShaderVariantManager::GetCacheHitRate() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Size total = m_cacheHits + m_cacheMisses;
    if (total == 0) {
        return 0.0f;
    }
    return static_cast<F32>(m_cacheHits) / static_cast<F32>(total);
}

void JzShaderVariantManager::ResetStatistics()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cacheHits   = 0;
    m_cacheMisses = 0;
}

} // namespace JzRE
