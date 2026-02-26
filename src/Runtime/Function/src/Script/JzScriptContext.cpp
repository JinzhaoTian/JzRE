/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Function/Script/JzScriptContext.h"
#include "JzRE/Runtime/Function/Script/JzScriptComponent.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"

#include <filesystem>

namespace JzRE {

// ---------------------------------------------------------------------------
// Initialization / Shutdown
// ---------------------------------------------------------------------------

void JzScriptContext::Initialize(JzWorld &world)
{
    m_world = &world;

    m_lua.open_libraries(
        sol::lib::base,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table,
        sol::lib::io);

    RegisterMathBindings();
    RegisterWorldBindings(world);
    RegisterLogBindings();

    JzRE_LOG_INFO("JzScriptContext: Lua {} initialized", LUA_VERSION);
}

void JzScriptContext::Shutdown()
{
    m_instances.clear();
    m_pathToEntities.clear();
    m_world = nullptr;
}

// ---------------------------------------------------------------------------
// Script Lifecycle
// ---------------------------------------------------------------------------

Bool JzScriptContext::LoadScript(JzEntity entity, const String &scriptPath)
{
    // Create a per-entity environment inheriting the global table
    sol::environment env(m_lua, sol::create, m_lua.globals());

    if (!ExecuteFile(scriptPath, env)) {
        return false;
    }

    std::filesystem::file_time_type mtime{};
    try {
        mtime = std::filesystem::last_write_time(scriptPath);
    } catch (...) {
    }

    ScriptInstance inst;
    inst.scriptPath     = scriptPath;
    inst.env            = std::move(env);
    inst.onStart        = inst.env.get<sol::protected_function>("OnStart");
    inst.onUpdate       = inst.env.get<sol::protected_function>("OnUpdate");
    inst.onStop         = inst.env.get<sol::protected_function>("OnStop");
    inst.lastWriteTime  = mtime;

    m_instances[entity] = std::move(inst);
    m_pathToEntities[scriptPath].push_back(entity);
    return true;
}

void JzScriptContext::UnloadScript(JzEntity entity)
{
    auto it = m_instances.find(entity);
    if (it == m_instances.end()) return;

    CallOnStop(entity);

    // Remove from path-to-entities mapping
    auto &entityList = m_pathToEntities[it->second.scriptPath];
    entityList.erase(std::remove(entityList.begin(), entityList.end(), entity),
                     entityList.end());
    if (entityList.empty()) {
        m_pathToEntities.erase(it->second.scriptPath);
    }

    m_instances.erase(it);
}

Bool JzScriptContext::HasScript(JzEntity entity) const
{
    return m_instances.find(entity) != m_instances.end();
}

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------

Bool JzScriptContext::CallOnStart(JzEntity entity)
{
    auto it = m_instances.find(entity);
    if (it == m_instances.end()) return false;

    auto &inst = it->second;
    if (!inst.onStart.valid()) return true; // OnStart is optional

    auto result = inst.onStart(static_cast<uint32_t>(entity));
    if (!result.valid()) {
        sol::error err = result;
        JzRE_LOG_WARN("JzScriptContext: OnStart error in '{}': {}", inst.scriptPath, err.what());
        return false;
    }
    return true;
}

Bool JzScriptContext::CallOnUpdate(JzEntity entity, F32 delta)
{
    auto it = m_instances.find(entity);
    if (it == m_instances.end()) return false;

    auto &inst = it->second;
    if (!inst.onUpdate.valid()) return true; // OnUpdate is optional

    auto result = inst.onUpdate(static_cast<uint32_t>(entity), delta);
    if (!result.valid()) {
        sol::error err = result;
        JzRE_LOG_WARN("JzScriptContext: OnUpdate error in '{}': {}", inst.scriptPath, err.what());
        return false;
    }
    return true;
}

void JzScriptContext::CallOnStop(JzEntity entity)
{
    auto it = m_instances.find(entity);
    if (it == m_instances.end()) return;

    auto &inst = it->second;
    if (!inst.onStop.valid()) return; // OnStop is optional

    auto result = inst.onStop(static_cast<uint32_t>(entity));
    if (!result.valid()) {
        sol::error err = result;
        JzRE_LOG_WARN("JzScriptContext: OnStop error in '{}': {}", inst.scriptPath, err.what());
    }
}

// ---------------------------------------------------------------------------
// Hot Reload
// ---------------------------------------------------------------------------

void JzScriptContext::CheckHotReload(F32 delta)
{
    m_timeSinceCheck += delta;
    if (m_timeSinceCheck < m_reloadInterval) return;
    m_timeSinceCheck = 0.0f;

    for (auto &[path, entities] : m_pathToEntities) {
        if (entities.empty()) continue;

        std::filesystem::file_time_type newMtime{};
        try {
            newMtime = std::filesystem::last_write_time(path);
        } catch (...) {
            continue;
        }

        // Compare against the first entity's cached time
        auto &refInst = m_instances[entities[0]];
        if (newMtime <= refInst.lastWriteTime) continue;

        JzRE_LOG_INFO("JzScriptContext: Hot-reloading '{}'", path);

        for (JzEntity e : entities) {
            auto it = m_instances.find(e);
            if (it == m_instances.end()) continue;

            auto &inst = it->second;

            // Re-execute the file into the existing environment
            if (!ExecuteFile(path, inst.env)) {
                // Keep old functions on failure
                continue;
            }

            // Refresh cached function handles
            inst.onStart       = inst.env.get<sol::protected_function>("OnStart");
            inst.onUpdate      = inst.env.get<sol::protected_function>("OnUpdate");
            inst.onStop        = inst.env.get<sol::protected_function>("OnStop");
            inst.lastWriteTime = newMtime;

            // Reset started so OnStart fires again on next frame
            if (m_world) {
                if (auto *comp = m_world->TryGetComponent<JzScriptComponent>(e)) {
                    comp->started = false;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Private: Bindings
// ---------------------------------------------------------------------------

void JzScriptContext::RegisterMathBindings()
{
    // ---- JzVec3 ----
    m_lua.new_usertype<JzVec3>(
        "Vec3",
        sol::constructors<JzVec3(), JzVec3(F32, F32, F32)>(),
        "x", &JzVec3::x,
        "y", &JzVec3::y,
        "z", &JzVec3::z,
        sol::meta_function::addition,       [](const JzVec3 &a, const JzVec3 &b) { return a + b; },
        sol::meta_function::subtraction,    [](const JzVec3 &a, const JzVec3 &b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const JzVec3 &a, const JzVec3 &b) { return a * b; },
            [](const JzVec3 &a, F32 s) { return a * s; }),
        "length",     &JzVec3::Length,
        "normalized", &JzVec3::Normalized,
        "dot",        &JzVec3::Dot,
        "cross",      &JzVec3::Cross,
        sol::meta_function::to_string, [](const JzVec3 &v) {
            return std::string("Vec3(") + std::to_string(v.x) + ", " +
                   std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        });

    // ---- JzVec2 ----
    m_lua.new_usertype<JzVec2>(
        "Vec2",
        sol::constructors<JzVec2(), JzVec2(F32, F32)>(),
        "x", &JzVec2::x,
        "y", &JzVec2::y);
}

void JzScriptContext::RegisterWorldBindings(JzWorld &world)
{
    // ---- JzTransformComponent ----
    m_lua.new_usertype<JzTransformComponent>(
        "TransformComponent",
        sol::no_constructor,
        "position", &JzTransformComponent::position,
        "rotation", &JzTransformComponent::rotation,
        "scale",    &JzTransformComponent::scale,
        "setDirty", &JzTransformComponent::SetDirty);

    // ---- world proxy table ----
    auto worldTable = m_lua.create_named_table("world");

    worldTable["createEntity"] = [&world]() -> uint32_t {
        return static_cast<uint32_t>(world.CreateEntity());
    };

    worldTable["destroyEntity"] = [&world](uint32_t id) {
        world.DestroyEntity(static_cast<JzEntity>(id));
    };

    worldTable["isValid"] = [&world](uint32_t id) -> bool {
        return world.IsValid(static_cast<JzEntity>(id));
    };

    worldTable["getTransform"] = [&world](uint32_t id) -> JzTransformComponent * {
        return world.TryGetComponent<JzTransformComponent>(static_cast<JzEntity>(id));
    };

    worldTable["hasTransform"] = [&world](uint32_t id) -> bool {
        return world.HasComponent<JzTransformComponent>(static_cast<JzEntity>(id));
    };
}

void JzScriptContext::RegisterLogBindings()
{
    auto logTable = m_lua.create_named_table("log");
    logTable["info"]  = [](const std::string &msg) { JzRE_LOG_INFO("{}", msg); };
    logTable["warn"]  = [](const std::string &msg) { JzRE_LOG_WARN("{}", msg); };
    logTable["error"] = [](const std::string &msg) { JzRE_LOG_ERROR("{}", msg); };
    logTable["debug"] = [](const std::string &msg) { JzRE_LOG_DEBUG("{}", msg); };
}

// ---------------------------------------------------------------------------
// Private: File execution
// ---------------------------------------------------------------------------

Bool JzScriptContext::ExecuteFile(const String &scriptPath, sol::environment &env)
{
    auto result = m_lua.script_file(scriptPath, env, sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        JzRE_LOG_WARN("JzScriptContext: Error loading '{}': {}", scriptPath, err.what());
        return false;
    }
    return true;
}

} // namespace JzRE
