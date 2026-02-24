/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

// Core types
#include "JzRE/Runtime/Resource/JzAssetId.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"

// Registry and cache
#include "JzRE/Runtime/Resource/JzAssetRegistry.h"
#include "JzRE/Runtime/Resource/JzLRUCache.h"

// Asset manager
#include "JzRE/Runtime/Resource/JzAssetManager.h"

// Common asset types (must be included for template instantiations)
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Runtime/Resource/JzModel.h"
#include "JzRE/Runtime/Resource/JzShader.h"

namespace JzRE {

/**
 * @brief Asset system version information
 */
struct JzAssetSystemVersion {
    static constexpr U32 Major = 1;
    static constexpr U32 Minor = 0;
    static constexpr U32 Patch = 0;

    static constexpr const char *GetVersionString()
    {
        return "1.0.0";
    }
};

/**
 * @brief Convenience function to create a default-configured asset manager
 *
 * @param maxMemoryMB Maximum cache memory in MB (default: 512)
 * @param workerCount Number of async worker threads (default: 2)
 * @return Configured and initialized asset manager
 */
inline std::unique_ptr<JzAssetManager> CreateAssetManager(Size maxMemoryMB = 512, Size workerCount = 2)
{
    JzAssetManagerConfig config;
    config.maxCacheMemoryMB = maxMemoryMB;
    config.asyncWorkerCount = workerCount;

    auto manager = std::make_unique<JzAssetManager>(config);
    manager->Initialize();

    return manager;
}

} // namespace JzRE
