/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/Script/JzScriptContext.h"

namespace JzRE {

/**
 * @brief ECS system that drives Lua script execution.
 *
 * Runs in the Logic phase and iterates every entity that carries a
 * JzScriptComponent.  On each frame it:
 *   1. Polls script files for modifications (hot reload).
 *   2. Loads and calls OnStart() for any newly added script entity.
 *   3. Calls OnUpdate(entity, delta) for all live script entities.
 *
 * Register this system after JzEventSystem so gameplay scripts can
 * dispatch events during the same frame they are raised:
 *
 * @code
 * m_scriptSystem = m_world->RegisterSystem<JzScriptSystem>();
 * JzServiceContainer::Provide<JzScriptSystem>(*m_scriptSystem);
 * @endcode
 *
 * Access from anywhere via:
 * @code
 * auto &ctx = JzServiceContainer::Get<JzScriptContext>();
 * @endcode
 */
class JzScriptSystem : public JzSystem {
public:
    JzScriptSystem() = default;

    /**
     * @brief Initialize the Lua VM and register engine bindings.
     */
    void OnInit(JzWorld &world) override;

    /**
     * @brief Poll hot reload, call OnStart for new scripts, OnUpdate for all.
     */
    void Update(JzWorld &world, F32 delta) override;

    /**
     * @brief Call OnStop on all scripts and shut down the Lua VM.
     */
    void OnShutdown(JzWorld &world) override;

    /**
     * @brief Script execution runs in the Logic phase.
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Logic;
    }

    /**
     * @brief Direct access to the Lua context (for tests / editor tooling).
     */
    JzScriptContext &GetContext()
    {
        return m_context;
    }

private:
    JzScriptContext m_context;
};

} // namespace JzRE
