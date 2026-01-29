/**
 * @file    JzShaderHotReloadSystem.h
 * @brief   ECS system for shader hot reloading
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_set>

#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"

namespace JzRE {

// Forward declarations
class JzWorld;
class JzAssetManager;
class JzShaderRegistry;
class JzShaderVariantManager;

/**
 * @brief System for hot reloading shader assets during development
 *
 * This system:
 * 1. Periodically checks tracked shader files for modifications
 * 2. Reloads modified shaders automatically
 * 3. Invalidates cached variants for recompilation
 * 4. Notifies material components to update their shader references
 *
 * Execution phase: Logic (runs before rendering)
 *
 * @note This system is intended for editor/development mode only.
 *       It should be disabled in release builds for performance.
 *
 * @example
 * @code
 * // Enable hot reload in editor mode
 * auto& hotReloadSystem = world.GetSystem<JzShaderHotReloadSystem>();
 * hotReloadSystem.SetEnabled(true);
 * hotReloadSystem.SetCheckInterval(1.0f); // Check every second
 * @endcode
 */
class JzShaderHotReloadSystem : public JzSystem {
public:
    /**
     * @brief Default constructor
     */
    JzShaderHotReloadSystem();

    /**
     * @brief Destructor
     */
    ~JzShaderHotReloadSystem() override;

    /**
     * @brief Initialize the system
     *
     * @param world The ECS world
     */
    void OnInit(JzWorld &world) override;

    /**
     * @brief Check for shader updates and reload if necessary
     *
     * @param world The ECS world
     * @param delta Delta time since last frame
     */
    void Update(JzWorld &world, F32 delta) override;

    /**
     * @brief Cleanup when system is destroyed
     *
     * @param world The ECS world
     */
    void OnShutdown(JzWorld &world) override;

    /**
     * @brief Get system execution phase
     *
     * @return JzSystemPhase::Logic
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Logic;
    }

    // ==================== Configuration ====================

    /**
     * @brief Set the interval between file modification checks
     *
     * @param seconds Interval in seconds (default: 1.0)
     */
    void SetCheckInterval(F32 seconds)
    {
        m_checkInterval = seconds;
    }

    /**
     * @brief Get the current check interval
     */
    [[nodiscard]] F32 GetCheckInterval() const
    {
        return m_checkInterval;
    }

    // ==================== Manual Control ====================

    /**
     * @brief Force an immediate check for shader updates
     *
     * Useful for triggering reload via editor UI.
     */
    void ForceCheck();

    /**
     * @brief Reload a specific shader immediately
     *
     * @param shaderHandle Handle to the shader to reload
     * @return true if reload succeeded
     */
    Bool ReloadShader(JzShaderAssetHandle shaderHandle);

    // ==================== Statistics ====================

    /**
     * @brief Get the number of shaders reloaded since startup
     */
    [[nodiscard]] Size GetReloadCount() const
    {
        return m_reloadCount;
    }

private:
    /**
     * @brief Check for shader file modifications
     *
     * @param world The ECS world
     */
    void CheckForUpdates(JzWorld &world);

    /**
     * @brief Notify all materials using a shader that it was reloaded
     *
     * @param shaderHandle Handle to the reloaded shader
     * @param world The ECS world
     */
    void NotifyShaderReloaded(JzShaderAssetHandle shaderHandle, JzWorld &world);

    /**
     * @brief Collect all shader handles currently in use
     *
     * @param world The ECS world
     * @return Set of shader handles
     */
    std::unordered_set<JzShaderAssetHandle, JzAssetHandle<JzShaderAsset>::Hash>
    CollectUsedShaders(JzWorld &world);

    JzWorld *m_world                 = nullptr; ///< Cached world pointer for context access
    F32      m_checkInterval         = 1.0f;    ///< Interval between checks (seconds)
    F32      m_timeSinceLastCheck    = 0.0f;    ///< Time since last check
    Size     m_reloadCount           = 0;       ///< Number of reloads performed
    Bool     m_forceCheckNextFrame   = false;   ///< Force check on next update
};

/**
 * @brief Tag component marking an entity as needing shader update
 *
 * Added by JzShaderHotReloadSystem when a shader is reloaded.
 * Should be processed by render systems to update GPU state.
 */
struct JzShaderDirtyTag { };

} // namespace JzRE
