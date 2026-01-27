/**
 * @file    JzAssetHeaders.h
 * @brief   Unified header for the asset management system
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 *
 * This header includes all components of the modern ECS-friendly asset system.
 * Include this single header to access all asset management functionality.
 *
 * @example
 * @code
 * #include "JzRE/Runtime/Resource/JzAssetHeaders.h"
 *
 * // Create and initialize asset manager
 * JzAssetManagerConfig config;
 * config.maxCacheMemoryMB = 512;
 * JzAssetManager assetManager(config);
 * assetManager.Initialize();
 *
 * // Register with service container
 * JzServiceContainer::Provide<JzAssetManager>(assetManager);
 *
 * // Load assets
 * auto meshHandle = assetManager.LoadSync<JzMesh>("cube.obj");
 * auto textureHandle = assetManager.LoadAsync<JzTexture>("diffuse.png");
 *
 * // Use in ECS
 * auto entity = world.CreateEntity();
 * world.AddComponent<JzMeshAssetComponent>(entity, meshHandle);
 * world.AddComponent<JzTransformComponent>(entity);
 *
 * // Register loading system
 * world.RegisterSystem<JzAssetLoadingSystem>();
 * @endcode
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
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

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
