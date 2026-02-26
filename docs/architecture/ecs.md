# EnTT ECS Integration

## Overview

JzRE uses EnTT as the ECS foundation.

`JzWorld` is the runtime ECS hub for:

- entity/component storage
- world context storage
- system registration and per-frame update

## Runtime Boundary (Mandatory)

Runtime ECS must remain editor-agnostic:

1. Runtime components/systems should model engine/game semantics.
2. Runtime contracts should not encode panel/product naming.
3. Editor behavior should consume runtime ECS via generic interfaces.

## Core Types

### `JzWorld`

`JzWorld` wraps `entt::registry` and provides:

- entity create/destroy/validity
- component add/remove/get/query
- context set/get
- system register/update

### `JzSystem`

`JzSystem` defines:

- `Update(JzWorld&, F32)` (required)
- `OnInit` / `OnShutdown` hooks
- `IsEnabled` / `SetEnabled`
- `GetPhase` metadata

## Quick Start

### Create World and Entities

```cpp
JzWorld world;
auto entity = world.CreateEntity();

if (world.IsValid(entity)) {
    // entity exists
}
```

### Add/Get/Remove Components

```cpp
auto &transform = world.AddComponent<JzTransformComponent>(entity);
transform.position = JzVec3(1.0f, 2.0f, 3.0f);

auto &camera = world.AddComponent<JzCameraComponent>(entity);

if (auto *input = world.TryGetComponent<JzInputStateComponent>(entity)) {
    // use input
}

world.RemoveComponent<JzCameraComponent>(entity);
```

### Query Entities

```cpp
auto view = world.View<JzTransformComponent, JzMeshAssetComponent>();
for (auto [e, transform, meshAsset] : view.each()) {
    // process
}
```

## World Context Usage

Context is stored in EnTT registry context.

```cpp
// Store a pointer-type context (current runtime event dispatcher pattern)
world.SetContext<JzEventSystem *>(eventSystemPtr);

// Retrieve (TryGetContext returns pointer to stored object)
if (auto **dispatcherPtr = world.TryGetContext<JzEventSystem *>()) {
    if (*dispatcherPtr) {
        (*dispatcherPtr)->Send(JzWindowClosedEvent{});
    }
}
```

## System Execution Model (Current Code)

### Registration

`RegisterSystem<T>()` currently:

- creates `std::shared_ptr<T>`
- appends to internal `m_systems` vector
- returns the shared pointer

### Update

`JzWorld::Update(delta)` currently:

- iterates `m_systems` in registration order
- calls `Update` for enabled systems

```cpp
for (auto &system : m_systems) {
    if (system && system->IsEnabled()) {
        system->Update(*this, delta);
    }
}
```

### Important Implementation Notes

- `JzSystemPhase` exists as system metadata.
- `JzWorld` does **not** currently schedule by phase groups.
- `UpdateLogic/UpdatePreRender/UpdateRender` APIs are not part of current `JzWorld` interface.
- `OnInit`/`OnShutdown` hooks are defined on systems, but are not automatically invoked by `JzWorld::RegisterSystem`/destruction in current implementation.

## Runtime System Order in `JzRERuntime`

Current runtime registers systems in this order:

1. `JzWindowSystem` (`Input` phase metadata)
2. `JzInputSystem` (`Input` phase metadata)
3. `JzEventSystem` (`Input` phase metadata)
4. `JzAssetSystem` (`Logic` phase metadata)
5. `JzScriptSystem` (`Logic` phase metadata) — Lua script execution
6. `JzCameraSystem` (`PreRender` phase metadata)
7. `JzLightSystem` (`PreRender` phase metadata)
8. `JzRenderSystem` (`Render` phase metadata)

Execution order is exactly this registration order because `JzWorld::Update` is linear.

## Main Loop Integration

Current `JzRERuntime::Run()` flow around ECS:

1. `m_windowSystem->PollWindowEvents()`
2. `OnUpdate(delta)`
3. `m_world->Update(delta)`
4. `OnRender(delta)`
5. `m_inputSystem->ClearFrameState(*m_world)`
6. `m_graphicsContext->Present()`

## Event and Input Flow

`JzEventSystem` is both:

- an ECS system in `JzWorld`
- a context pointer (`JzEventSystem*`) used by other systems

Current flow:

```text
Window backend callbacks/polling
    -> JzWindowSystem updates JzWindowStateComponent/JzInputStateComponent
    -> JzWindowSystem emits window events via JzEventSystem*
    -> JzInputSystem consumes input state, updates higher-level input components
    -> JzInputSystem emits input/action events via JzEventSystem*
    -> JzEventSystem::Update dispatches queued events
```

## Rendering-Relevant ECS Data

### Rendered entity requirements (`JzRenderSystem`)

Entity must have:

- `JzTransformComponent`
- `JzMeshAssetComponent`
- `JzMaterialAssetComponent`
- `JzAssetReadyTag`

Visibility filtering uses:

- `JzOverlayRenderTag`
- `JzIsolatedRenderTag`
- target visibility mask (`MainScene`, `Overlay`, `Isolated`)

### Camera/Light dependencies

- `JzCameraSystem` updates `JzCameraComponent` view/projection
- `JzLightSystem` collects light data from light components
- `JzRenderSystem` reads camera/light results during rendering

## Common Components

### Transform and Motion

- `JzTransformComponent`
- `JzVelocityComponent`

### Camera

- `JzCameraComponent`
- `JzOrbitControllerComponent`
- `JzMainCameraTag`
- `JzCameraInputComponent`
- `JzCameraInputStateComponent`

### Lighting

- `JzDirectionalLightComponent`
- `JzPointLightComponent`
- `JzSpotLightComponent`

### Rendering/Asset Binding

- `JzMeshAssetComponent`
- `JzMaterialAssetComponent`
- `JzTextureAssetComponent`
- `JzAssetReadyTag`
- `JzAssetLoadingTag`
- `JzAssetLoadFailedTag`

### Input and Window

- `JzInputStateComponent`
- `JzMouseInputComponent`
- `JzKeyboardInputComponent`
- `JzInputActionComponent`
- `JzWindowStateComponent`
- `JzWindowEventQueueComponent`
- `JzPrimaryWindowTag`

### Scripting

- `JzScriptComponent` — attaches a Lua script (`.lua` file) to an entity

  | Field | Type | Description |
  |---|---|---|
  | `scriptPath` | `String` | Path to the `.lua` file (resolved via asset search paths) |
  | `started` | `Bool` | Set by `JzScriptSystem` once `OnStart` has been called |

  The script file may define any combination of:
  ```lua
  function OnStart(entity)   end  -- called once on first frame
  function OnUpdate(entity, dt) end  -- called every Logic frame
  function OnStop(entity)    end  -- called on entity destruction / system shutdown
  ```
  Entities are passed as `uint32_t` integers. Use the `world` global table to operate on them.

## Editor Interop with Runtime ECS

Editor uses runtime ECS/render interfaces without editor-specific runtime coupling:

- view panels register runtime render targets
- panel camera entities and feature masks are updated via render-system APIs
- panel texture display comes from `JzRenderOutput`

This keeps runtime ECS reusable for non-editor applications.

## Source References

- `src/Runtime/Function/include/JzRE/Runtime/Function/ECS/JzWorld.h`
- `src/Runtime/Function/include/JzRE/Runtime/Function/ECS/JzWorld.inl`
- `src/Runtime/Function/src/ECS/JzWorld.cpp`
- `src/Runtime/Function/include/JzRE/Runtime/Function/ECS/JzSystem.h`
- `src/Runtime/Interface/src/JzRERuntime.cpp`
- `src/Runtime/Function/src/ECS/JzWindowSystem.cpp`
- `src/Runtime/Function/src/ECS/JzInputSystem.cpp`
- `src/Runtime/Function/include/JzRE/Runtime/Function/Event/JzEventSystem.h`
- `src/Runtime/Function/src/ECS/JzRenderSystem.cpp`
- `src/Runtime/Function/include/JzRE/Runtime/Function/Script/JzScriptComponent.h`
- `src/Runtime/Function/include/JzRE/Runtime/Function/Script/JzScriptContext.h`
- `src/Runtime/Function/include/JzRE/Runtime/Function/Script/JzScriptSystem.h`
