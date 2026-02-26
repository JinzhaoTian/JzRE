/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Component that attaches a Lua script to an entity.
 *
 * Add this component to any entity to drive its behavior through a Lua
 * script.  The script file is resolved relative to the asset search paths
 * registered with JzAssetSystem.
 *
 * The script must define one or more of the following global functions:
 *   function OnStart(entity)          -- called once on the first frame
 *   function OnUpdate(entity, dt)     -- called every Logic-phase frame
 *   function OnStop(entity)           -- called when the entity is destroyed
 *                                        or the system shuts down
 *
 * Entities are passed as plain integers (uint32_t) matching the underlying
 * entt::entity value.  Use the global `world` table to operate on them.
 */
struct JzScriptComponent {
    /// Path to the Lua file (relative path resolved via asset search paths,
    /// or an absolute path).
    String scriptPath;

    /// Tracks whether OnStart has already been called for this entity.
    /// Set by JzScriptSystem — do not modify manually.
    Bool started{false};
};

} // namespace JzRE
