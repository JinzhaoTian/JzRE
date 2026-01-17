# EnTT ECS Integration

## Overview

JzRE uses EnTT as its primary ECS implementation. The ECS is now the core architecture for the rendering pipeline in `JzRERuntime`.

## Quick Start

### Include Header

```cpp
#include "JzRE/Runtime/Function/ECS/EnTT/JzEnttECS.h"
```

### Create a World

```cpp
JzRE::JzEnttWorld world;
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

```cpp
class MySystem : public JzRE::JzEnttSystem {
public:
    void OnInit(JzEnttWorld& world) override {
        // Called when system is registered
    }

    void Update(JzEnttWorld& world, F32 delta) override {
        auto view = world.View<JzTransformComponent, JzVelocityComponent>();

        for (auto [entity, transform, velocity] : view.each()) {
            transform.position += velocity.velocity * delta;
        }
    }

    void OnShutdown(JzEnttWorld& world) override {
        // Called when system is destroyed
    }
};
```

### Registering Systems

```cpp
JzRE::JzEnttWorld world;

// Register systems in update order
auto cameraSystem = world.RegisterSystem<JzEnttCameraSystem>();
auto lightSystem = world.RegisterSystem<JzEnttLightSystem>();
auto renderSystem = world.RegisterSystem<JzEnttRenderSystem>();

// Update all systems (in registration order)
world.Update(deltaTime);
```

---

## JzRERuntime ECS Architecture

The `JzRERuntime` class uses ECS as its primary rendering architecture:

```
JzRERuntime
  └── JzEnttWorld (entity/component storage, system management)
        ├── JzEnttCameraSystem (camera matrix updates, orbit control)
        ├── JzEnttLightSystem (light data collection)
        └── JzEnttRenderSystem (framebuffer, pipeline, entity rendering)
```

### System Update Order

Systems are updated in the order they are registered:

1. **JzEnttCameraSystem** - Processes input, computes view/projection matrices
2. **JzEnttLightSystem** - Collects light data from light entities
3. **JzEnttRenderSystem** - Renders all entities with Transform + Mesh + Material

### Main Loop Flow

```cpp
void JzRERuntime::Run() {
    OnStart();

    while (IsRunning()) {
        m_window->PollEvents();

        // Update camera aspect ratio
        m_cameraSystem->SetAspectRatio(aspect);

        OnUpdate(deltaTime);

        // Update frame size
        m_renderSystem->SetFrameSize(frameSize);

        // ECS update: Camera -> Light -> Render
        m_renderSystem->BeginFrame();
        m_world->Update(deltaTime);
        m_renderSystem->EndFrame();

        // Blit to screen (standalone) or provide texture (Editor)
        if (ShouldBlitToScreen()) {
            m_renderSystem->BlitToScreen(width, height);
        }

        OnRender(deltaTime);
        m_window->SwapBuffers();
    }

    OnStop();
}
```

---

## Available Systems

| System | Description |
|--------|-------------|
| `JzEnttCameraSystem` | Updates camera matrices, handles orbit controller input |
| `JzEnttLightSystem` | Collects light data for rendering |
| `JzEnttRenderSystem` | Manages framebuffer, renders entities with mesh/material |
| `JzEnttMoveSystem` | Updates position based on velocity |
| `JzEnttSceneSystem` | Updates world transforms in hierarchy |

---

## Components

### Core Components

- `JzTransformComponent` - Position, rotation, scale
- `JzVelocityComponent` - Velocity vector
- `JzMeshComponent` - Reference to mesh resource
- `JzMaterialComponent` - Reference to material resource
- `JzSceneNodeComponent` - Scene graph node
- `JzHierarchyComponent` - Parent/children relationships

### Rendering Components

| Component | Description |
|-----------|-------------|
| `JzEnttCameraComponent` | Full camera state (position, rotation, fov, near/far, matrices) |
| `JzEnttOrbitControllerComponent` | Orbit camera controller (target, yaw, pitch, distance, sensitivity) |
| `JzEnttDirectionalLightComponent` | Directional light (direction, color, intensity) |
| `JzEnttPointLightComponent` | Point light (color, intensity, range, attenuation) |
| `JzEnttSpotLightComponent` | Spot light (direction, color, intensity, cutoff angles) |

### Tag Components

- `JzActiveTag` - Tag for active entities
- `JzStaticTag` - Tag for static entities
- `JzPendingDestroyTag` - Mark for deferred destruction
- `JzRenderableTag` - Mark entity as renderable
- `JzMainCameraTag` - Mark main camera entity

### Utility Components

- `JzNameComponent` - Human-readable entity name
- `JzUUIDComponent` - Unique identifier for serialization

---

## Model Spawning

Use `JzEnttModelSpawner` to convert model resources into ECS entities:

```cpp
// Load a model
auto model = std::make_shared<JzModel>("path/to/model.obj");
model->Load();

// Spawn as ECS entities (one per mesh)
auto entities = JzEnttModelSpawner::SpawnModel(world, model);

// Each entity has:
// - JzTransformComponent
// - JzMeshComponent
// - JzMaterialComponent
// - JzRenderableTag

// Clean up when done
JzEnttModelSpawner::DestroyEntities(world, entities);
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
├── include/JzRE/Runtime/Function/ECS/
│   ├── JzComponent.h              # Shared component definitions
│   ├── JzEntity.h                 # Entity type
│   └── EnTT/
│       ├── JzEnttECS.h            # Convenience header
│       ├── JzEnttEntity.h         # Entity type definitions
│       ├── JzEnttWorld.h          # Core world class
│       ├── JzEnttWorld.inl        # Template implementations
│       ├── JzEnttSystem.h         # System base class
│       ├── JzEnttComponents.h     # Component re-exports + tags
│       ├── JzEnttRenderComponents.h  # Camera, light, rendering components
│       ├── JzEnttModelSpawner.h   # Model to entity conversion
│       └── Systems/
│           ├── JzEnttCameraSystem.h
│           ├── JzEnttLightSystem.h
│           ├── JzEnttRenderSystem.h
│           ├── JzEnttMoveSystem.h
│           └── JzEnttSceneSystem.h
└── src/ECS/EnTT/
    ├── JzEnttWorld.cpp
    ├── JzEnttModelSpawner.cpp
    └── Systems/
        ├── JzEnttCameraSystem.cpp
        ├── JzEnttLightSystem.cpp
        └── JzEnttRenderSystem.cpp
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
