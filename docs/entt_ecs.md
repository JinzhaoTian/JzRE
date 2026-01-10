# EnTT ECS Integration

## Overview

The JzRE project now supports two ECS implementations:

1. **Original ECS** (`JzRE/ECS/`) - Custom sparse-set based implementation
2. **EnTT ECS** (`JzRE/EnttECS/`) - Modern, high-performance ECS using the EnTT library

Both implementations coexist, allowing gradual migration and comparison.

## Quick Start

### Include Header

```cpp
#include "JzRE/EnttECS/JzEnttECS.h"
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
    void Update(JzEnttWorld& world, F32 delta) override {
        auto view = world.View<JzTransformComponent, JzVelocityComponent>();
        
        for (auto [entity, transform, velocity] : view.each()) {
            transform.position += velocity.velocity * delta;
        }
    }
};
```

### Registering Systems

```cpp
JzRE::JzEnttWorld world;

// Register a system
auto moveSystem = world.RegisterSystem<JzEnttMoveSystem>();
auto renderSystem = world.RegisterSystem<JzEnttRenderSystem>(device);

// Update all systems
world.Update(deltaTime);
```

## Available Systems

| System | Description |
|--------|-------------|
| `JzEnttMoveSystem` | Updates position based on velocity |
| `JzEnttSceneSystem` | Updates world transforms in hierarchy |
| `JzEnttRenderSystem` | Renders entities with mesh/material |

## Components

The EnTT ECS reuses the same component definitions from the original ECS:

- `JzTransformComponent` - Position, rotation, scale
- `JzVelocityComponent` - Velocity vector
- `JzMeshComponent` - Reference to mesh resource
- `JzMaterialComponent` - Reference to material resource
- `JzSceneNodeComponent` - Scene graph node
- `JzCameraComponent` - Camera properties
- And more...

### EnTT-Specific Components

- `JzActiveTag` - Tag for active entities
- `JzStaticTag` - Tag for static entities
- `JzPendingDestroyTag` - Mark for deferred destruction
- `JzNameComponent` - Human-readable entity name
- `JzUUIDComponent` - Unique identifier for serialization

## Advanced Usage

### Direct Registry Access

For advanced EnTT features:

```cpp
entt::registry& registry = world.GetRegistry();

// Use EnTT signals
registry.on_construct<JzTransformComponent>().connect<&OnTransformCreated>();

// Create groups for optimized iteration
auto group = registry.group<JzTransformComponent>(entt::get<JzVelocityComponent>);
```

## Migration Guide

### From Original ECS to EnTT

| Original ECS | EnTT ECS |
|-------------|----------|
| `JzEntityManager manager` | `JzEnttWorld world` |
| `manager.CreateEntity()` | `world.CreateEntity()` |
| `manager.AddComponent<T>(e, ...)` | `world.AddComponent<T>(e, ...)` |
| `manager.GetComponent<T>(e)` | `world.GetComponent<T>(e)` |
| `manager.View<A, B>()` returns `vector` | `world.View<A, B>()` returns EnTT view |
| `JzSystem::Update(manager, dt)` | `JzEnttSystem::Update(world, dt)` |

### Key Differences

1. **Entity Type**: EnTT uses `entt::entity` (includes version) vs `U32`
2. **View**: EnTT views are lazy and more efficient than returning `std::vector`
3. **Null Entity**: Use `INVALID_ENTT_ENTITY` or `entt::null`
4. **Entity Recycling**: EnTT automatically recycles destroyed entity IDs

## Module Structure

```
src/EnttECS/
├── CMakeLists.txt
├── include/JzRE/EnttECS/
│   ├── JzEnttECS.h              # Convenience header
│   ├── JzEnttEntity.h           # Entity type definitions
│   ├── JzEnttWorld.h            # Core world class
│   ├── JzEnttWorld.inl          # Template implementations
│   ├── JzEnttSystem.h           # System base class
│   ├── JzEnttComponents.h       # Component definitions
│   └── Systems/
│       ├── JzEnttMoveSystem.h
│       ├── JzEnttSceneSystem.h
│       └── JzEnttRenderSystem.h
└── src/
    └── JzEnttWorld.cpp
```

## Dependencies

- **EnTT** (v3.14+): Header-only ECS library
  - Added via vcpkg: `"entt": "3.14.0"`
  - CMake: `find_package(EnTT CONFIG REQUIRED)` + `target_link_libraries(... EnTT::EnTT)`

## References

- [EnTT Documentation](https://github.com/skypjack/entt/wiki)
- [EnTT Tutorial](https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system)
