/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <sol/sol.hpp>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"

namespace JzRE {

class JzWorld;

/**
 * @brief Manages the Lua VM and per-entity script environments.
 *
 * JzScriptContext owns a single sol::state (Lua VM) and maintains one
 * sol::environment per scripted entity so that global variables defined
 * in different scripts do not collide.
 *
 * Engine API exposed to every Lua script:
 *   Vec3(x, y, z)          -- construct a JzVec3 userdata
 *   log.info/warn/error     -- write to JzLogger
 *   world.createEntity()    -- returns entity id (integer)
 *   world.destroyEntity(id)
 *   world.getTransform(id)  -- returns JzTransformComponent userdata or nil
 *   world.hasTransform(id)  -- returns bool
 *
 * Entities are passed to Lua callbacks as plain integers (uint32_t) that
 * map 1-to-1 with the underlying entt::entity value.
 *
 * Hot reload:
 *   Call CheckHotReload() every frame.  When a script file's modification
 *   time changes, the file is reloaded into the existing per-entity
 *   environment and the entity's started flag is reset so OnStart fires
 *   again on the next frame.
 */
class JzScriptContext {
public:
    JzScriptContext()  = default;
    ~JzScriptContext() = default;

    JzScriptContext(const JzScriptContext &)            = delete;
    JzScriptContext &operator=(const JzScriptContext &) = delete;

    /**
     * @brief Initialize the Lua state and register all engine bindings.
     *
     * Must be called before any other method.
     *
     * @param world The ECS world used by world.* Lua API calls.
     */
    void Initialize(JzWorld &world);

    /**
     * @brief Release all Lua resources and clear per-entity state.
     */
    void Shutdown();

    // ==================== Script Lifecycle ====================

    /**
     * @brief Load (or reload) a Lua script file for the given entity.
     *
     * Creates a new per-entity environment, executes the script, and
     * caches references to OnStart / OnUpdate / OnStop if defined.
     *
     * @param entity    Target entity.
     * @param scriptPath Path to the .lua file (absolute or relative to CWD).
     * @return True on success, false if the file cannot be read or contains
     *         a Lua error (error is logged via JzLogger).
     */
    Bool LoadScript(JzEntity entity, const String &scriptPath);

    /**
     * @brief Unload a script from an entity, calling OnStop first.
     *
     * @param entity Target entity.
     */
    void UnloadScript(JzEntity entity);

    /**
     * @brief Returns true if a script is currently loaded for the entity.
     */
    Bool HasScript(JzEntity entity) const;

    // ==================== Callbacks ====================

    /**
     * @brief Invoke the script's OnStart(entity) function.
     *
     * @return False if a Lua error occurred (already logged).
     */
    Bool CallOnStart(JzEntity entity);

    /**
     * @brief Invoke the script's OnUpdate(entity, delta) function.
     *
     * @return False if a Lua error occurred (already logged).
     */
    Bool CallOnUpdate(JzEntity entity, F32 delta);

    /**
     * @brief Invoke the script's OnStop(entity) function (if defined).
     */
    void CallOnStop(JzEntity entity);

    // ==================== Hot Reload ====================

    /**
     * @brief Accumulate delta and check file timestamps every reload interval.
     *
     * When a script file's modification time has advanced, the file is
     * re-executed into its existing environment and the entity's started
     * flag is reset via the JzScriptComponent.
     *
     * @param delta Frame delta time in seconds.
     */
    void CheckHotReload(F32 delta);

    /**
     * @brief Set how often (in seconds) file timestamps are polled.
     *
     * Default is 0.5 seconds.
     */
    void SetHotReloadInterval(F32 seconds)
    {
        m_reloadInterval = seconds;
    }

    // ==================== Access ====================

    /**
     * @brief Direct access to the underlying Lua state (tests / advanced).
     */
    sol::state &GetState()
    {
        return m_lua;
    }

private:
    // ==================== Private helpers ====================

    void RegisterMathBindings();
    void RegisterWorldBindings(JzWorld &world);
    void RegisterLogBindings();

    /**
     * @brief Execute a Lua file into an existing environment.
     *
     * @return True on success.
     */
    Bool ExecuteFile(const String &scriptPath, sol::environment &env);

    // ==================== Per-entity script state ====================

    struct ScriptInstance {
        String                           scriptPath;
        sol::environment                 env{sol::lua_nil};
        sol::protected_function          onStart{sol::lua_nil};
        sol::protected_function          onUpdate{sol::lua_nil};
        sol::protected_function          onStop{sol::lua_nil};
        std::filesystem::file_time_type  lastWriteTime{};
    };

    sol::state                                          m_lua;
    std::unordered_map<JzEntity, ScriptInstance>        m_instances;

    /// script path → entities using that script (for hot-reload fanout)
    std::unordered_map<String, std::vector<JzEntity>>   m_pathToEntities;

    JzWorld *m_world{nullptr};
    F32      m_reloadInterval{0.5f};
    F32      m_timeSinceCheck{0.0f};
};

} // namespace JzRE
