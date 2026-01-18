# JzRE Architecture Overview

## Introduction

JzRE (Jinzhao's Real-time game Engine) is a cross-platform, multi-graphics-API game engine built with modern C++20. The engine is designed with a clean layered architecture that separates concerns and enables easy extension and maintenance.

---

## Core Design Philosophy

### Layered Architecture

The engine follows a strict **Runtime + Editor** architecture where dependencies flow strictly downward:

```
┌─────────────────────────────────────────────────────────────┐
│              JzREInstance (JzRE Executable)                 │
├─────────────────────────────────────────────────────────────┤
│                     Editor (JzEditor)                       │
│              ImGui-based development tools                  │
├─────────────────────────────────────────────────────────────┤
│                  JzRERuntime (Interface)                    │
│           Runtime application framework interface           │
├─────────────────────────────────────────────────────────────┤
│                    Function Layer                           │
│                ECS, Input, Window                           │
├─────────────────────────────────────────────────────────────┤
│                    Resource Layer                           │
│       ResourceManager, Asset Factories, Caching             │
├─────────────────────────────────────────────────────────────┤
│                    Platform Layer                           │
│     RHI Abstraction, Graphics Backends, OS APIs             │
├─────────────────────────────────────────────────────────────┤
│                      Core Layer                             │
│        Types, Math, Threading, Events, Logging              │
└─────────────────────────────────────────────────────────────┘
```

### Design Principles

| Principle                  | Description                                              |
| -------------------------- | -------------------------------------------------------- |
| **Separation of Concerns** | Each layer has a single responsibility                   |
| **Dependency Inversion**   | Upper layers depend on abstractions, not implementations |
| **Data-Oriented Design**   | ECS with cache-friendly component pools                  |
| **Command Pattern**        | RHI uses command lists for deferred execution            |
| **Service Locator**        | Dependency injection via `JzServiceContainer`            |

---

## Layer Overview

### Core Layer

The foundation of the engine with zero dependencies on other modules.

**Key Components:**

- `JzRETypes.h` - Fundamental type definitions (U32, F32, Bool, String, etc.)
- `JzVector.h`, `JzMatrix.h` - Math primitives and operations
- `JzThreadPool.h`, `JzTaskQueue.h` - Concurrent execution infrastructure
- `JzEvent.h` - Event system for decoupled communication
- `JzServiceContainer.h` - Dependency injection container
- `JzLogger.h` - Logging infrastructure (via spdlog)

📄 See: [Module Structure](module.md)

### Platform Layer

Abstracts platform-specific functionality and graphics APIs.

**Key Components:**

- **RHI (Render Hardware Interface)** - Graphics API abstraction
- `JzDevice` - Unified device interface for resource creation
- `JzRHICommandList` - Command buffer for deferred rendering
- `JzGPU*Object` - GPU resource wrappers (Buffer, Texture, Shader, etc.)
- `JzFileDialog` - Cross-platform file dialogs

**Supported Backends:**

- ✅ OpenGL 3.3+
- 🔜 Vulkan (planned)

📄 See: [RHI Design](rhi.md)

### Resource Layer

Manages asset lifecycle with automatic caching and reference counting.

**Key Components:**

- `JzResourceManager` - Central resource access point
- `JzResourceFactory` - Factory pattern for resource creation
- Resource types: `JzTexture`, `JzMesh`, `JzModel`, `JzShader`, `JzMaterial`, `JzFont`

**Features:**

- Automatic caching with `weak_ptr`
- Reference-counted unloading
- Search path management
- Type-safe resource access

📄 See: [Resource Layer Design](resource.md)

### Function Layer

High-level engine systems built on lower layers.

**Subsystems:**

| Subsystem     | Description                                   |
| ------------- | --------------------------------------------- |
| **Rendering** | `JzRHIRenderer` - High-level render commands  |
| **Scene**     | `JzScene`, `JzActor`, `JzWorld` - Scene graph |
| **ECS**       | `JzEntityManager`, Systems, Components        |
| **Input**     | `JzInputManager` - Keyboard/mouse handling    |
| **Window**    | `JzWindow` - GLFW window wrapper              |

📄 See: [ECS Integration](ecs.md), [Rendering Pipeline](rendering_pipeline.md)

### Editor Layer

Development tools built with ImGui.

**Key Components:**

- `JzEditor` - Main editor loop
- `JzPanelsManager` - Panel layout management
- `JzUIManager` - ImGui integration
- Panels: `JzSceneView`, `JzHierarchy`, `JzAssetBrowser`, `JzConsole`

---

## Key Patterns

### Command Pattern (RHI)

```cpp
// Record commands
auto cmdList = device->CreateCommandList("MainPass");
cmdList->Begin();
cmdList->Clear(clearParams);
cmdList->BindPipeline(pipeline);
cmdList->BindVertexArray(vertexArray);
cmdList->DrawIndexed(drawParams);
cmdList->End();

// Execute later
device->ExecuteCommandList(cmdList);
```

### Service Container (Dependency Injection)

```cpp
// Provide service
JzServiceContainer::Provide<JzResourceManager>(manager);

// Consume service
auto& resMgr = JzServiceContainer::Get<JzResourceManager>();
```

### Factory Pattern (Resources)

```cpp
// Register factories
resourceManager.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());

// Get resource (auto-created and cached)
auto texture = resourceManager.GetResource<JzTexture>("textures/player.png");
```

### Entity-Component-System

```cpp
// Create entity and add components
auto entity = world.CreateEntity();
world.AddComponent<JzTransformComponent>(entity);
world.AddComponent<JzMeshComponent>(entity);

// Query and process
auto view = world.View<JzTransformComponent, JzVelocityComponent>();
for (auto [entity, transform, velocity] : view.each()) {
    transform.position += velocity.velocity * deltaTime;
}
```

---

## Build Targets

| Target              | Type              | Description              |
| ------------------- | ----------------- | ------------------------ |
| `JzRuntimeCore`     | Static Library    | Core utilities           |
| `JzRuntimePlatform` | Static Library    | RHI + Graphics backends  |
| `JzRuntimeResource` | Static Library    | Resource management      |
| `JzRuntimeFunction` | Static Library    | High-level systems       |
| `JzRERuntime`       | Interface Library | Links all runtime layers |
| `JzEditor`          | Static Library    | Editor logic             |
| `JzREInstance`      | Executable        | Main application         |

---

## External Dependencies

Managed via vcpkg:

| Library           | Purpose                    |
| ----------------- | -------------------------- |
| **glfw3**         | Window and input           |
| **glad**          | OpenGL loader              |
| **imgui**         | Editor UI (docking branch) |
| **assimp**        | 3D model loading           |
| **stb**           | Image loading              |
| **gtest**         | Unit testing               |
| **nlohmann-json** | JSON parsing               |
| **spdlog**        | Logging                    |
| **fmt**           | String formatting          |
| **freetype**      | Font rendering             |
| **entt**          | ECS library                |

---

## Documentation Index

| Document                                    | Description                              |
| ------------------------------------------- | ---------------------------------------- |
| [Architecture Layers](layers.md)            | Detailed layer descriptions and diagrams |
| [Module Structure](module.md)               | Directory layout and CMake configuration |
| [RHI Design](rhi.md)                        | Render Hardware Interface specification  |
| [Resource Layer](resource.md)               | Asset management system                  |
| [ECS Integration](ecs.md)                   | Entity-Component-System usage guide      |
| [Rendering Pipeline](rendering_pipeline.md) | Data flow from ECS to GPU                |
| [Threading Roadmap](threading.md)           | Multi-threading evolution plan           |

---

## Quick Start

### Build

```bash
# Configure (uses vcpkg for dependencies)
cmake -B build

# Build all targets
cmake --build build

# Run main application
./build/JzRE/JzRE

# Run tests
cd build && ctest --output-on-failure
```

### Include Convention

```cpp
// Runtime modules
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzResourceManager.h"
#include "JzRE/Runtime/Function/ECS/JzEntityManager.h"

// Editor modules
#include "JzRE/Editor/JzEditor.h"
```

---

## Future Roadmap

### Near-term

- [ ] Vulkan backend implementation
- [ ] Async resource loading
- [ ] Material system improvements

### Mid-term

- [ ] Parallel ECS systems
- [ ] Scene serialization
- [ ] Physics integration

### Long-term

- [ ] Multi-threaded command recording
- [ ] Render graph
- [ ] Ray tracing support

📄 See: [Threading Roadmap](threading.md) for detailed evolution plan.
