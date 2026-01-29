/**
 * @file    JzShaderHotReloadSystem.cpp
 * @brief   Implementation of JzShaderHotReloadSystem
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzShaderHotReloadSystem.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

namespace JzRE {

JzShaderHotReloadSystem::JzShaderHotReloadSystem() = default;

JzShaderHotReloadSystem::~JzShaderHotReloadSystem() = default;

void JzShaderHotReloadSystem::OnInit(JzWorld &world)
{
    m_world = &world;
    JzRE_LOG_INFO("JzShaderHotReloadSystem: Initialized with check interval {}s", m_checkInterval);
}

void JzShaderHotReloadSystem::Update(JzWorld &world, F32 delta)
{
    if (!IsEnabled()) {
        return;
    }

    m_timeSinceLastCheck += delta;

    // Check if it's time to look for updates
    if (m_forceCheckNextFrame || m_timeSinceLastCheck >= m_checkInterval) {
        CheckForUpdates(world);
        m_timeSinceLastCheck  = 0.0f;
        m_forceCheckNextFrame = false;
    }
}

void JzShaderHotReloadSystem::OnShutdown(JzWorld &world)
{
    JzRE_LOG_INFO("JzShaderHotReloadSystem: Shutdown, reloaded {} shaders total", m_reloadCount);
}

void JzShaderHotReloadSystem::ForceCheck()
{
    m_forceCheckNextFrame = true;
}

Bool JzShaderHotReloadSystem::ReloadShader(JzShaderAssetHandle shaderHandle)
{
    // Get asset manager from world context
    JzAssetManager **assetManagerPtr = m_world ? m_world->TryGetContext<JzAssetManager*>() : nullptr;
    if (!assetManagerPtr || !*assetManagerPtr) {
        JzRE_LOG_WARN("JzShaderHotReloadSystem: AssetManager not available");
        return false;
    }
    JzAssetManager *assetManager = *assetManagerPtr;

    if (!assetManager->IsInitialized()) {
        return false;
    }

    JzShaderAsset *shader = assetManager->Get(shaderHandle);
    if (!shader) {
        JzRE_LOG_WARN("JzShaderHotReloadSystem: Shader not found for reload");
        return false;
    }

    Bool success = shader->Reload();
    if (success) {
        ++m_reloadCount;
        JzRE_LOG_INFO("JzShaderHotReloadSystem: Reloaded shader '{}'", shader->GetName());
    } else {
        JzRE_LOG_ERROR("JzShaderHotReloadSystem: Failed to reload shader '{}'", shader->GetName());
    }

    return success;
}

void JzShaderHotReloadSystem::CheckForUpdates(JzWorld &world)
{
    // Get asset manager from world context
    JzAssetManager **assetManagerPtr = world.TryGetContext<JzAssetManager*>();
    if (!assetManagerPtr || !*assetManagerPtr) {
        return;
    }
    JzAssetManager *assetManager = *assetManagerPtr;

    if (!assetManager->IsInitialized()) {
        return;
    }

    // Collect all shaders currently in use
    auto usedShaders = CollectUsedShaders(world);

    // Check each shader for modifications
    for (auto shaderHandle : usedShaders) {
        JzShaderAsset *shader = assetManager->Get(shaderHandle);
        if (!shader) {
            continue;
        }

        if (shader->NeedsReload()) {
            JzRE_LOG_INFO("JzShaderHotReloadSystem: Detected change in shader '{}'",
                          shader->GetName());

            if (shader->Reload()) {
                ++m_reloadCount;
                NotifyShaderReloaded(shaderHandle, world);
                JzRE_LOG_INFO("JzShaderHotReloadSystem: Successfully reloaded shader '{}'",
                              shader->GetName());
            } else {
                JzRE_LOG_ERROR("JzShaderHotReloadSystem: Failed to reload shader '{}'",
                               shader->GetName());
            }
        }
    }
}

void JzShaderHotReloadSystem::NotifyShaderReloaded(JzShaderAssetHandle shaderHandle,
                                                   JzWorld            &world)
{
    // Mark all entities with this shader as dirty
    auto shaderView = world.View<JzShaderAssetComponent>();
    for (auto entity : shaderView) {
        auto &shaderComp = shaderView.get<JzShaderAssetComponent>(entity);
        if (shaderComp.shaderHandle == shaderHandle) {
            // Reset ready state to trigger recompilation
            shaderComp.isReady       = false;
            shaderComp.cachedVariant = nullptr;

            // Add dirty tag for render systems
            world.AddOrReplaceComponent<JzShaderDirtyTag>(entity);

            JzRE_LOG_DEBUG("JzShaderHotReloadSystem: Marked entity {} for shader update",
                           static_cast<U32>(entity));
        }
    }

    // Also check material components that reference this shader
    auto materialView = world.View<JzMaterialAssetComponent>();
    for (auto entity : materialView) {
        auto &matComp = materialView.get<JzMaterialAssetComponent>(entity);
        if (matComp.shaderHandle == shaderHandle) {
            // Reset cached variant
            matComp.cachedShaderVariant = nullptr;

            // Add dirty tag
            world.AddOrReplaceComponent<JzShaderDirtyTag>(entity);

            JzRE_LOG_DEBUG("JzShaderHotReloadSystem: Marked material entity {} for shader update",
                           static_cast<U32>(entity));
        }
    }
}

std::unordered_set<JzShaderAssetHandle, JzAssetHandle<JzShaderAsset>::Hash>
JzShaderHotReloadSystem::CollectUsedShaders(JzWorld &world)
{
    std::unordered_set<JzShaderAssetHandle, JzAssetHandle<JzShaderAsset>::Hash> usedShaders;

    // Collect from shader components
    auto shaderView = world.View<JzShaderAssetComponent>();
    for (auto entity : shaderView) {
        auto &shaderComp = shaderView.get<JzShaderAssetComponent>(entity);
        if (shaderComp.shaderHandle.IsValid()) {
            usedShaders.insert(shaderComp.shaderHandle);
        }
    }

    // Collect from material components
    auto materialView = world.View<JzMaterialAssetComponent>();
    for (auto entity : materialView) {
        auto &matComp = materialView.get<JzMaterialAssetComponent>(entity);
        if (matComp.shaderHandle.IsValid()) {
            usedShaders.insert(matComp.shaderHandle);
        }
    }

    return usedShaders;
}

} // namespace JzRE
