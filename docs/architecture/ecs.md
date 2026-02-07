# EnTT ECS Integration

## Overview

JzRE uses EnTT as its primary ECS implementation. The ECS is now the core architecture for the rendering pipeline in `JzRERuntime`.

## Quick Start

### Include Header

```cpp
#include "JzRE/Runtime/Function/ECS/EnTT/JzECS.h"
```

### Create a World

```cpp
JzRE::JzWorld world;
```

### Entity Management

```cpp
// Create an entity
auto entity = world.CreateEntity();

// Destroy an entity
world.DestroyEntity(entity);

// Check if entity is valid
if (world.IsValid(entity)) {
    // Entity still exists
}
```

### Component Management

```cpp
// Add a component
auto& transform = world.AddComponent<JzTransformComponent>(entity);
transform.position = JzVec3(1.0f, 2.0f, 3.0f);

// Get a component
auto& velocity = world.GetComponent<JzVelocityComponent>(entity);

// Try to get a component (returns nullptr if not present)
if (auto* mesh = world.TryGetComponent<JzMeshComponent>(entity)) {
    // Use mesh
}

// Check if entity has a component
if (world.HasComponent<JzTransformComponent>(entity)) {
    // Has transform
}

// Remove a component
world.RemoveComponent<JzVelocityComponent>(entity);

// For tag components (empty structs), use direct registry access
world.GetRegistry().emplace<JzRenderableTag>(entity);
```

### Querying Entities (Views)

The EnTT view is the most efficient way to iterate over entities:

```cpp
// Create a view for entities with Transform and Velocity
auto view = world.View<JzTransformComponent, JzVelocityComponent>();

// Iterate using structured bindings (recommended)
for (auto [entity, transform, velocity] : view.each()) {
    transform.position += velocity.velocity * deltaTime;
}

// Alternative: iterate over entities and get components
for (auto entity : view) {
    auto& transform = view.get<JzTransformComponent>(entity);
    auto& velocity = view.get<JzVelocityComponent>(entity);
    // ...
}
```

### Creating Systems

Systems can specify their execution phase via `GetPhase()`:

```cpp
class MySystem : public JzRE::JzSystem {
public:
    void OnInit(JzWorld& world) override {
        // Called when system is registered
    }

    void Update(JzWorld& world, F32 delta) override {
        auto view = world.View<JzTransformComponent, JzVelocityComponent>();

        for (auto [entity, transform, velocity] : view.each()) {
            transform.position += velocity.velocity * delta;
        }
    }

    void OnShutdown(JzWorld& world) override {
        // Called when system is destroyed
    }

    // Override to specify execution phase (default: Logic)
    JzSystemPhase GetPhase() const override {
        return JzSystemPhase::Logic;  // or PreRender, Render
    }
};
```

### System Phases

Systems are categorized into 8 phases for proper synchronization:

| Phase Group   | Phase      | Enum                        | Purpose                                   |
| ------------- | ---------- | --------------------------- | ----------------------------------------- |
| **Logic**     | Input      | `JzSystemPhase::Input`      | Input processing, event handling          |
|               | Physics    | `JzSystemPhase::Physics`    | Physics simulation, collision detection   |
|               | Animation  | `JzSystemPhase::Animation`  | Skeletal animation, blend trees           |
|               | Logic      | `JzSystemPhase::Logic`      | General game logic, AI, scripts           |
| **PreRender** | PreRender  | `JzSystemPhase::PreRender`  | Camera matrices, light collection         |
|               | Culling    | `JzSystemPhase::Culling`    | Frustum culling, occlusion, LOD selection |
| **Render**    | RenderPrep | `JzSystemPhase::RenderPrep` | Batch building, instance data preparation |
|               | Render     | `JzSystemPhase::Render`     | Actual GPU draw calls                     |

Helper functions are available to check phase groups:

- `IsLogicPhase(phase)` - Returns true for Input, Physics, Animation, Logic
- `IsPreRenderPhase(phase)` - Returns true for PreRender, Culling
- `IsRenderPhase(phase)` - Returns true for RenderPrep, Render

### Registering Systems

```cpp
JzRE::JzWorld world;

// Register systems in execution order
// Logic phases (can run parallel with GPU)
world.RegisterSystem<InputSystem>();       // Input
world.RegisterSystem<PhysicsSystem>();     // Physics
world.RegisterSystem<AnimationSystem>();   // Animation
world.RegisterSystem<AISystem>();          // Logic

// PreRender phases (after sync point)
auto cameraSystem = world.RegisterSystem<JzCameraSystem>();  // PreRender
auto lightSystem = world.RegisterSystem<JzLightSystem>();    // PreRender
world.RegisterSystem<CullingSystem>();     // Culling

// Render phases
world.RegisterSystem<BatchBuildingSystem>();   // RenderPrep
world.RegisterSystem<InstanceDataSystem>();    // RenderPrep
auto renderSystem = world.RegisterSystem<JzRenderSystem>();  // Render

// Update all systems (in registration order)
world.Update(deltaTime);

// Or update by phase group:
world.UpdateLogic(deltaTime);     // All logic phases (Input -> Physics -> Animation -> Logic)
world.UpdatePreRender(deltaTime); // All pre-render phases (PreRender -> Culling)
world.UpdateRender(deltaTime);    // All render phases (RenderPrep -> Render)

// Or update a specific phase:
world.UpdatePhase(JzSystemPhase::Physics, deltaTime);  // Physics only
```

---

## JzRERuntime ECS Architecture

The `JzRERuntime` class uses ECS as its primary rendering architecture with phase-based system execution:

```
JzRERuntime
  └── JzWorld (entity/component storage, system management)
        ├── Input Systems        (JzSystemPhase::Input)
        │     └── JzInputSystem (processes raw input, updates input components)
        ├── Logic Systems        (JzSystemPhase::Logic)
        │     └── User-defined logic systems
        ├── PreRender Systems    (JzSystemPhase::PreRender)
        │     ├── JzCameraSystem (camera matrix updates, orbit control)
        │     └── JzLightSystem (light data collection)
        └── Render Systems       (JzSystemPhase::Render)
              └── JzRenderSystem (framebuffer, pipeline, entity rendering)
```

### System Update Order

Systems are grouped by phase and executed in registration order within each phase:

1. **Input Phase** - Input processing and component updates (JzInputSystem)
2. **Logic Phase** - Game logic, physics, AI, animations (user-defined systems)
3. **PreRender Phase** - Camera, lights, culling (JzCameraSystem, JzLightSystem)
4. **Render Phase** - GPU rendering (JzRenderSystem)

### Main Loop Flow

The main loop executes in 8 distinct phases for proper synchronization:

```cpp
void JzRERuntime::Run() {
    OnStart();

    while (IsRunning()) {
        // Phase 1: Frame Start - Input and Window Events
        m_window->PollEvents();

        // Phase 2: Async Processing - Start Background Tasks
        _SignalWorkerFrame(frameData);

        // Phase 3: ECS Logic Update (can run parallel with GPU)
        _UpdateECSLogic(frameData);  // m_world->UpdateLogic()

        // User Logic Hook
        OnUpdate(deltaTime);

        // Phase 4: Sync Point - Wait for Background Tasks
        _WaitForWorkerComplete();

        // Phase 5: ECS Pre-Render Update (camera, lights, culling)
        _UpdateECSPreRender(frameData);  // m_world->UpdatePreRender()

        // Phase 6: ECS Render Update
        _UpdateECSRender(frameData);  // m_world->UpdateRender()

        // Phase 7: Execute Rendering
        _ExecuteRendering(frameData);  // BeginFrame, EndFrame, BlitToScreen

        // Phase 8: Frame End
        _FinishFrame(frameData);  // SwapBuffers, ClearEvents
    }

    OnStop();
}
```

### Phase Separation Benefits

1. **Parallel Execution**: Logic systems can run in parallel with GPU work
2. **Clear Synchronization**: Explicit sync point ensures background tasks complete before rendering
3. **Data Isolation**: PreRender phase prepares render data after logic updates
4. **Extensibility**: Easy to add new systems by specifying their phase

---

## Event System Integration

The ECS event system bridges `JzWindowSystem` and `JzInputSystem` with `JzEventSystem`, enabling any system to subscribe to typed window/input events. The event system is stored as a singleton in the `JzWorld` context.

### Event Flow

All events flow through `JzEventSystem`. There are no public delegates on
`JzWindowSystem` — the backend's internal `JzDelegate` callbacks are an implementation
detail of the Platform layer.

```
GLFW Callbacks
    → IWindowBackend delegates (internal, Platform layer)
        → JzWindowSystem accumulates callback data (e.g. scroll delta)

JzWindowSystem::Update()
    ├── Polls backend → JzWindowStateComponent
    ├── Polls backend + applies callback data → JzInputStateComponent
    └── Change detection → emits window ECS events
            (JzWindowResizedEvent, JzWindowFocusEvent, etc.)
    ↓
JzInputSystem::Update()
    ├── Syncs legacy components (JzMouseInputComponent, etc.)
    ├── Updates action values (JzInputActionComponent)
    └── Emits input ECS events (JzKeyEvent, JzMouseButtonEvent, etc.)
    ↓
JzEventSystem (stored in JzWorld context)
    └── Dispatches all queued events to registered handlers
    ↓
Any System → RegisterHandler<JzKeyEvent>(...) to receive events
```

### Window Events (`JzWindowEvents.h`)

| Event                                | Fields                         | Emitted When                  |
| ------------------------------------ | ------------------------------ | ----------------------------- |
| `JzWindowResizedEvent`               | `size`, `oldSize`              | Window size changes           |
| `JzWindowFramebufferResizedEvent`    | `size`                         | Framebuffer resizes (HiDPI)   |
| `JzWindowMovedEvent`                 | `position`                     | Window position changes       |
| `JzWindowFocusEvent`                 | `focused`                      | Focus gained/lost             |
| `JzWindowIconifiedEvent`             | `iconified`                    | Minimized/restored            |
| `JzWindowMaximizedEvent`             | `maximized`                    | Maximized/restored            |
| `JzWindowClosedEvent`                | `forced`                       | Close requested               |
| `JzFileDroppedEvent`                 | `filePaths`, `dropPosition`    | File dropped on window        |
| `JzWindowContentScaleChangedEvent`   | `scale`                        | DPI scale changes             |

### Input Events (`JzInputEvents.h`)

| Event                          | Fields                              | Emitted When                     |
| ------------------------------ | ----------------------------------- | -------------------------------- |
| `JzKeyEvent`                   | `key`, `scancode`, `action`, `mods` | Key pressed/released             |
| `JzMouseButtonEvent`           | `button`, `action`, `mods`, `position` | Mouse button pressed/released |
| `JzMouseMoveEvent`             | `position`, `delta`                 | Mouse moved (non-zero delta)     |
| `JzMouseScrollEvent`           | `offset`, `position`                | Scroll wheel used                |
| `JzMouseEnterEvent`            | `entered`                           | Cursor enters/leaves window      |
| `JzInputActionTriggeredEvent`  | `actionName`, `value`               | Action triggered this frame      |
| `JzInputActionReleasedEvent`   | `actionName`, `duration`            | Action released this frame       |

### Subscribing to Events

```cpp
void MySystem::OnInit(JzWorld &world) {
    // Event system is stored in world context (not service container)
    auto &eventSystem = world.GetContext<JzEventSystem>();

    eventSystem.RegisterHandler<JzKeyEvent>([](const JzKeyEvent &event) {
        if (event.key == JzEKeyCode::Escape && event.action == JzEKeyAction::Pressed) {
            // Handle escape key
        }
    });

    eventSystem.RegisterHandler<JzWindowResizedEvent>([](const JzWindowResizedEvent &event) {
        // Handle window resize
    });
}
```

### System Registration Order

The event system is initialized in the world context before registering other systems:

```cpp
// Initialize event system in world context
m_world->SetContext<JzEventSystem>(std::make_unique<JzEventSystem>());

// Register systems in order
m_windowSystem = m_world->RegisterSystem<JzWindowSystem>();
m_inputSystem  = m_world->RegisterSystem<JzInputSystem>();
m_assetSystem  = m_world->RegisterSystem<JzAssetSystem>();
// ... remaining systems
```

---

## Available Systems

| System             | Phase     | Description                                                       |
| ------------------ | --------- | ----------------------------------------------------------------- |
| `JzWindowSystem`   | Input     | Polls backend, syncs window/input state, emits window events      |
| `JzInputSystem`    | Input     | Processes input, syncs legacy components, emits input events      |
| `JzAssetSystem`    | Logic     | Asset loading, hot reload, shader variant management              |
| `JzMoveSystem`     | Logic     | Entity movement and basic physics                                 |
| `JzCameraSystem`   | PreRender | Updates camera matrices, reads input components for orbit control |
| `JzLightSystem`    | PreRender | Collects light data for rendering                                 |
| `JzRenderSystem`   | Render    | Manages framebuffer, renders entities with mesh/material          |

**Note:** `JzEventSystem` is stored in `JzWorld` context (accessed via `world.GetContext<JzEventSystem>()`), not registered as a system.

**Camera Aspect Policy:** `JzCameraSystem` updates the aspect ratio from the primary window only for the main camera. View-specific cameras (e.g., editor preview cameras) are expected to set their own aspect based on the render target size.

---

## Components

Components are organized into per-system header files.

### Entity Components (`JzEntityComponents.h`)

| Component              | Description                          |
| ---------------------- | ------------------------------------ |
| `JzActiveTag`          | Tag for active entities              |
| `JzStaticTag`          | Tag for static entities              |
| `JzPendingDestroyTag`  | Mark for deferred destruction        |
| `JzNameComponent`      | Human-readable entity name           |
| `JzUUIDComponent`      | Unique identifier for serialization  |

### Transform Components (`JzTransformComponents.h`)

| Component                | Description                              |
| ------------------------ | ---------------------------------------- |
| `JzTransformComponent`   | Position, rotation, scale, cached matrix |
| `JzVelocityComponent`    | Velocity vector                          |

### Camera Components (`JzCameraComponents.h`)

| Component                      | Description                                                         |
| ------------------------------ | ------------------------------------------------------------------- |
| `JzCameraComponent`            | Full camera state (position, rotation, fov, near/far, matrices)     |
| `JzOrbitControllerComponent`   | Orbit camera controller (target, yaw, pitch, distance, sensitivity) |
| `JzMainCameraTag`              | Tag for main camera entity                                          |
| `JzCameraInputComponent`       | Legacy processed camera input (orbit/pan, mouse delta, scroll)      |

### Light Components (`JzLightComponents.h`)

| Component                     | Description                                             |
| ----------------------------- | ------------------------------------------------------- |
| `JzDirectionalLightComponent` | Directional light (direction, color, intensity)         |
| `JzPointLightComponent`       | Point light (color, intensity, range, attenuation)      |
| `JzSpotLightComponent`        | Spot light (direction, color, intensity, cutoff angles)  |

### Render Components (`JzRenderComponents.h`)

| Component              | Description                              |
| ---------------------- | ---------------------------------------- |
| `JzMeshComponent`      | Mesh geometry data and GPU resources     |
| `JzMaterialComponent`  | PBR material properties and textures     |
| `JzRenderableTag`      | Tag to mark entity as renderable         |
| `JzSkyboxComponent`    | Skybox tag                               |
| `JzGridComponent`      | Grid visualization parameters            |
| `JzGizmoComponent`     | Editor gizmo type and selection state    |

### Spatial Components (`JzSpatialComponents.h`)

| Component                    | Description                              |
| ---------------------------- | ---------------------------------------- |
| `JzBoundingBoxComponent`     | Axis-aligned bounding box (min, max)     |
| `JzBoundingSphereComponent`  | Bounding sphere (center, radius)         |
| `JzBoundsComponent`          | Local and world bounds                   |
| `JzSpatialComponent`         | Spatial partition (position, grid cell)   |
| `JzStreamingComponent`       | Streaming load state                     |

### Input Components (`JzInputComponents.h`)

| Component                       | Description                                                      |
| ------------------------------- | ---------------------------------------------------------------- |
| `JzInputStateComponent`         | Full input state (keyboard, mouse, gamepad)                      |
| `JzInputActionComponent`        | Action bindings and input contexts                               |
| `JzCameraInputStateComponent`   | Enhanced camera input signals                                    |
| `JzMouseInputComponent`         | Legacy mouse position, delta, button states                      |
| `JzKeyboardInputComponent`      | Legacy common key states (WASD, arrows, modifiers)               |

### Window Components (`JzWindowComponents.h`)

| Component                       | Description                                          |
| ------------------------------- | ---------------------------------------------------- |
| `JzWindowStateComponent`        | Window properties, state, and dirty flags            |
| `JzDisplayComponent`            | Display/monitor info and video modes                 |
| `JzWindowEventQueueComponent`   | Queued window events for the frame                   |
| `JzPrimaryWindowTag`            | Tag for the primary window entity                    |
| `JzPersistentWindowTag`         | Tag for windows that cannot be closed by user        |

### Asset Components (`JzAssetComponents.h`)

| Component                    | Description                                               |
| ---------------------------- | --------------------------------------------------------- |
| `JzMeshAssetComponent`       | Reference to mesh asset for deferred loading              |
| `JzMaterialAssetComponent`   | Reference to material asset (with shader variants)        |
| `JzTextureAssetComponent`    | Reference to texture asset                                |
| `JzModelAssetComponent`      | Reference to model asset                                  |
| `JzShaderAssetComponent`     | Reference to shader asset (with defines)                  |
| `JzAssetLoadingTag`          | Tag indicating asset is currently loading                 |
| `JzAssetReadyTag`            | Tag indicating all assets are GPU-ready                   |
| `JzAssetLoadFailedTag`       | Tag indicating asset load failed                          |
| `JzShaderDirtyTag`           | Tag for shader hot reload detection                       |
| `JzTextureDirtyTag`          | Tag for texture hot reload detection                      |
| `JzMaterialDirtyTag`         | Tag for material hot reload detection                     |
| `JzAssetReferenceComponent`  | Asset lifecycle tracking                                  |
| `JzRenderQueueComponent`     | Render queue classification (Background/Opaque/Transparent/Overlay) |
| `JzInstanceGroupComponent`   | Instance rendering group for batching                     |

---

## Model Spawning

Use `JzModelSpawner` to convert model resources into ECS entities:

```cpp
// Load a model
auto model = std::make_shared<JzModel>("path/to/model.obj");
model->Load();

// Spawn as ECS entities (one per mesh)
auto entities = JzModelSpawner::SpawnModel(world, model);

// Each entity has:
// - JzTransformComponent
// - JzMeshComponent
// - JzMaterialComponent
// - JzRenderableTag

// Clean up when done
JzModelSpawner::DestroyEntities(world, entities);
```

---

## Editor Integration

The Editor accesses rendering output via the render system:

```cpp
// Get the color texture for ImGui display
auto colorTexture = runtime.GetRenderSystem()->GetColorTexture();

// Display in ImGui viewport
ImGui::Image(
    (void*)(intptr_t)colorTexture->GetHandle(),
    ImVec2(width, height)
);
```

Override `ShouldBlitToScreen()` to control display:

```cpp
class MyEditor : public JzRERuntime {
protected:
    Bool ShouldBlitToScreen() const override {
        return false; // Editor handles display via ImGui
    }
};
```

---

## Input Processing Architecture

JzRE uses a **pure ECS input architecture** where all input flows through components and systems:

### JzInputSystem (ECS System)

The `JzInputSystem` processes raw input state and distributes it to ECS components:

- Runs in the **Input phase** (first logic phase)
- Reads from `JzInputStateComponent` (populated by `JzWindowSystem` via GLFW callbacks)
- Updates higher-level input components on entities (`JzMouseInputComponent`, `JzKeyboardInputComponent`, `JzCameraInputStateComponent`)
- Processes input action bindings (`JzInputActionComponent`)
- Emits typed ECS events (`JzKeyEvent`, `JzMouseButtonEvent`, etc.)

```cpp
// Systems read from input components — all data is ECS-driven
void MyCameraSystem::Update(JzWorld& world, F32 delta) {
    auto view = world.View<JzCameraComponent, JzCameraInputComponent>();

    for (auto [entity, camera, input] : view.each()) {
        if (input.orbitActive) {
            // Process orbit input using component data
            ApplyOrbit(camera, input.mouseDelta);
        }
    }
}

// Non-ECS code (e.g., Editor panels) can query JzInputStateComponent directly
auto& world = JzServiceContainer::Get<JzWorld>();
auto inputView = world.View<JzInputStateComponent, JzPrimaryWindowTag>();
for (auto entity : inputView) {
    auto& input = world.GetComponent<JzInputStateComponent>(entity);
    if (input.keyboard.IsKeyPressed(JzEKeyCode::W)) { /* ... */ }
}
```

### Input Flow Diagram

```
Hardware Input
    ↓
GLFW Callbacks → IWindowBackend (internal delegates)
    ↓
JzWindowSystem (polls backend + applies callback data)
    ↓
JzInputStateComponent (per-window ECS component)
    ↓
JzInputSystem (Input Phase)
    ├── Syncs higher-level components (JzMouseInputComponent, etc.)
    ├── Updates action values (JzInputActionComponent)
    └── Emits ECS events (JzKeyEvent, JzMouseButtonEvent, etc.)
    ↓
JzEventSystem (in JzWorld context) → registered handlers
    ↓
Game Systems (CameraSystem, PlayerSystem, etc.)
```

### Benefits of This Architecture

1. **Pure ECS**: All input state flows through components — no service-based singletons
2. **Testability**: Systems can be tested with mock input components
3. **Data-Driven**: Input is just another component type
4. **Extensibility**: Easy to add input mappings, action systems, etc.
5. **Clear Dependencies**: Systems depend on components, not global services
6. **Unified Access**: Both ECS systems and Editor code query the same `JzInputStateComponent`

### Input Component Patterns

#### Pattern 1: Direct Input Components

For simple input queries:

```cpp
auto view = world.View<PlayerComponent, JzKeyboardInputComponent>();
for (auto [entity, player, input] : view.each()) {
    if (input.w) player.position.z -= speed * delta;
    if (input.s) player.position.z += speed * delta;
}
```

#### Pattern 2: Processed Input Components

For complex input behavior (like camera control):

```cpp
// JzInputSystem processes raw input
void JzInputSystem::UpdateCameraInput(JzWorld& world) {
    auto view = world.View<JzCameraInputComponent, JzMouseInputComponent>();
    for (auto [entity, cameraInput, mouseInput] : view.each()) {
        cameraInput.orbitActive = mouseInput.leftButtonDown;
        cameraInput.mouseDelta = mouseInput.positionDelta;
    }
}

// CameraSystem reads processed input
void JzCameraSystem::Update(JzWorld& world, F32 delta) {
    auto view = world.View<JzCameraComponent, JzCameraInputComponent>();
    for (auto [entity, camera, input] : view.each()) {
        if (input.orbitActive) {
            ApplyOrbit(camera, input.mouseDelta);
        }
    }
}
```

### Future Extensions

The input architecture supports future enhancements:

- **Input Actions**: Map keys to abstract actions ("Jump", "Fire", etc.)
- **Input Contexts**: Different input mappings per game state (menu, gameplay, etc.)
- **Input Buffering**: Store input events for frame-perfect execution
- **Input Replay**: Record and replay input for debugging/testing
- **Dead Zones**: Configure analog input dead zones per-component
- **Key Rebinding**: Remap keys at runtime by modifying input components

---

## Advanced Usage

### Direct Registry Access

For advanced EnTT features:

```cpp
entt::registry& registry = world.GetRegistry();

// Use EnTT signals
registry.on_construct<JzTransformComponent>().connect<&OnTransformCreated>();

// Create groups for optimized iteration
auto group = registry.group<JzTransformComponent>(entt::get<JzVelocityComponent>);

// Add tag components (empty structs)
registry.emplace<JzRenderableTag>(entity);
```

### Accessing Systems in JzRERuntime

```cpp
// Get systems from runtime
auto cameraSystem = runtime.GetCameraSystem();
auto lightSystem = runtime.GetLightSystem();
auto renderSystem = runtime.GetRenderSystem();

// Access camera data
JzMat4 viewMatrix = cameraSystem->GetViewMatrix();
JzMat4 projMatrix = cameraSystem->GetProjectionMatrix();
JzVec3 cameraPos = cameraSystem->GetCameraPosition();

// Access light data
JzVec3 lightDir = lightSystem->GetPrimaryLightDirection();
JzVec3 lightColor = lightSystem->GetPrimaryLightColor();
```

---

## Module Structure

```
src/Runtime/Function/
├── include/JzRE/Runtime/Function/
│   ├── ECS/
│   │   ├── JzEntity.h                  # Entity type definitions (handle with generation)
│   │   ├── JzWorld.h                   # Core world class (wraps entt::registry)
│   │   ├── JzWorld.inl                 # Template implementations
│   │   ├── JzSystem.h                  # System base class with phases
│   │   │
│   │   ├── JzEntityComponents.h        # Entity metadata (tags, name, UUID)
│   │   ├── JzTransformComponents.h     # Transform, velocity
│   │   ├── JzCameraComponents.h        # Camera, orbit controller, camera input
│   │   ├── JzLightComponents.h         # Directional, point, spot lights
│   │   ├── JzRenderComponents.h        # Mesh, material, renderable, skybox, grid, gizmo
│   │   ├── JzSpatialComponents.h       # Bounding volumes, spatial partition, streaming
│   │   ├── JzInputComponents.h         # Input state, actions, legacy mouse/keyboard
│   │   ├── JzWindowComponents.h        # Window state, display, events
│   │   ├── JzAssetComponents.h         # Asset refs, ready/loading/dirty tags
│   │   │
│   │   ├── JzCameraSystem.h
│   │   ├── JzLightSystem.h
│   │   ├── JzRenderSystem.h
│   │   ├── JzInputSystem.h
│   │   ├── JzWindowSystem.h
│   │   ├── JzAssetSystem.h             # Asset loading, hot reload integration
│   │   └── JzMoveSystem.h
│   │
│   └── Event/
│       ├── JzEventSystem.h             # Main event dispatcher (stored in JzWorld context)
│       ├── JzEventQueue.h              # Lock-free event queue
│       ├── JzIEventHandler.h           # Event handler interface
│       ├── JzECSEvent.h                # ECS event wrapper & type registry
│       ├── JzWindowEvents.h            # Window events (resize, focus, close, etc.)
│       ├── JzInputEvents.h             # Input events (key, mouse, scroll)
│       └── JzPlatformEventAdapter.h    # Adapter from platform to ECS events
│
└── src/ECS/
    ├── JzWorld.cpp
    ├── JzCameraSystem.cpp, JzLightSystem.cpp, JzRenderSystem.cpp
    ├── JzInputSystem.cpp, JzWindowSystem.cpp
    ├── JzAssetSystem.cpp
    └── JzMoveSystem.cpp
```

---

## Dependencies

- **EnTT** (v3.14+): Header-only ECS library
  - Added via vcpkg: `"entt": "3.14.0"`
  - CMake: `find_package(EnTT CONFIG REQUIRED)` + `target_link_libraries(... EnTT::EnTT)`

---

## References

- [EnTT Documentation](https://github.com/skypjack/entt/wiki)
- [EnTT Tutorial](https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system)
