/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Function/Script/JzScriptSystem.h"
#include "JzRE/Runtime/Function/Script/JzScriptComponent.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"

namespace JzRE {

void JzScriptSystem::OnInit(JzWorld &world)
{
    m_context.Initialize(world);
    JzServiceContainer::Provide<JzScriptContext>(m_context);
    JzRE_LOG_INFO("JzScriptSystem: initialized");
}

void JzScriptSystem::Update(JzWorld &world, F32 delta)
{
    // 1. Check for file modifications and hot-reload changed scripts
    m_context.CheckHotReload(delta);

    // 2. Iterate all entities with a JzScriptComponent
    auto view = world.View<JzScriptComponent>();
    for (auto [entity, script] : view.each()) {
        if (script.scriptPath.empty()) continue;

        if (!script.started) {
            // Load the script the first time (or after hot reload reset)
            if (!m_context.HasScript(entity)) {
                if (!m_context.LoadScript(entity, script.scriptPath)) {
                    // Loading failed — mark started to avoid retrying every frame
                    script.started = true;
                    continue;
                }
            }
            m_context.CallOnStart(entity);
            script.started = true;
        } else {
            m_context.CallOnUpdate(entity, delta);
        }
    }
}

void JzScriptSystem::OnShutdown(JzWorld &world)
{
    // Call OnStop for every entity that still has a running script
    auto view = world.View<JzScriptComponent>();
    for (auto [entity, script] : view.each()) {
        if (script.started) {
            m_context.CallOnStop(entity);
        }
    }

    m_context.Shutdown();
    JzServiceContainer::Remove<JzScriptContext>();
    JzRE_LOG_INFO("JzScriptSystem: shutdown");
}

} // namespace JzRE
