# JzRE Architecture Overview

## Introduction

JzRE (Jinzhao's Real-time game Engine) is a cross-platform, multi-graphics-API game engine built with modern C++20. The engine is designed with a clean layered architecture that separates concerns and enables easy extension and maintenance.

---

## Core Design Philosophy

### Layered Architecture

The engine follows a strict **Runtime + Editor** architecture where dependencies flow strictly downward:

```
┌─────────────────────────────────────────────────────────────┐
│              JzREEditor (JzRE Executable)                   │
├─────────────────────────────────────────────────────────────┤
│                     Editor (JzEditorUI)                     │
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

### Runtime-Editor Boundary (Mandatory)

Runtime (`src/Runtime/**`) is editor-agnostic engine code. Editor (`src/Editor/**`) is a runtime consumer.

Mandatory rules:

1. Runtime public APIs must not expose editor concepts or panel-level names (`Editor`, `SceneView`, `GameView`, `AssetView`, etc.).
2. Runtime modules must not depend on editor UI/tooling libraries (for example, `imgui` and editor panel classes).
3. Editor-specific behavior must be injected through generic runtime extension points (callbacks, descriptors, plugin-like pass registration), not embedded as runtime-specific semantics.
4. Any new runtime abstraction must remain reusable by standalone game applications that do not link the editor.

---

## Layer Overview

### Core Layer

The foundation of the engine with zero dependencies on other modules.

**Key Components:**

- `JzRETypes.h` - Fundamental type definitions (U32, F32, Bool, String, etc.)
- `JzVector.h`, `JzMatrix.h`, `JzVertex.h` - Math primitives and operations
- `JzClock.h` - Timing and frame delta calculations
- `JzThreadPool.h`, `JzTaskQueue.h` - Concurrent execution infrastructure
- `JzPlatformEvent.h`, `JzPlatformEventQueue.h` - Platform-agnostic event types
- `JzServiceContainer.h` - Dependency injection container
- `JzLogger.h`, `JzLogSink.h` - Logging infrastructure (via spdlog)
- `JzDelegate.h` - Callback/delegate pattern
- `JzFileSystemUtils.h` - File I/O utilities

📄 See: [Module Structure](module.md)

### Platform Layer

Abstracts platform-specific functionality and graphics APIs.

**Key Components:**

- **RHI (Render Hardware Interface)** - Graphics API abstraction
- `JzGraphicsContext` - Owns device and presentation/context switching
- `JzDevice` - Unified device interface for resource creation
- `JzDeviceFactory` - Device creation factory (backend selection)
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

- `JzAssetManager` - Central asset management with generation-based handles
- `JzAssetRegistry<T>` - Per-type asset storage with O(1) access
- `JzLRUCache` - LRU cache with memory budget management
- `JzResourceFactory` - Factory pattern for resource creation
- Resource types: `JzTexture`, `JzMesh`, `JzModel`, `JzShaderAsset`, `JzMaterial`, `JzFont`

**Features:**

- Type-safe handles with generation tracking (`JzAssetHandle<T>`)
- Synchronous and asynchronous loading
- LRU cache with configurable memory budget
- Shader variants and hot reload support
- Search path management

📄 See: [Resource Layer Design](resource.md), [Asset System](asset_system.md)

### Function Layer

High-level engine systems built on lower layers.

**Subsystems:**

| Subsystem     | Description                                                                                                                                                                                          |
| ------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Scene**     | `JzActor` - Legacy scene graph (optional)                                                                                                                                                            |
| **ECS**       | `JzWorld`, Systems, Components (EnTT-based)                                                                                                                                                          |
| **Event**     | `JzEventSystem` - ECS event dispatcher (stored in JzWorld context)                                                                                                                                   |
| **Input**     | `JzInputSystem` - ECS-based keyboard/mouse/gamepad input processing                                                                                                                                  |
| **Window**    | `JzWindowSystem` - ECS-integrated GLFW window management                                                                                                                                             |
| **Asset**     | `JzAssetSystem` - Asset loading, hot reload, ECS integration                                                                                                                                         |
| **Rendering** | `JzRenderSystem`, `JzRenderGraph`, `JzRenderOutput` - ECS-driven rendering orchestration where logical `RenderTarget` descriptors map to concrete `RenderOutput` resources |

📄 See: [ECS Integration](ecs.md), [Rendering Pipeline](rendering_pipeline.md)

### Editor Layer

Development tools built with ImGui.

**Key Components:**

- `JzEditorUI` - Main editor loop
- `JzPanelsManager` - Panel layout management
- `JzUIManager` - ImGui integration
- Panels: `JzSceneView`, `JzHierarchy`, `JzAssetBrowser`, `JzConsole`

`JzSceneView` enables editor helper rendering by default (procedural skybox + world axis helper), controlled through editor settings.
The editor must consume runtime public interfaces and must not require runtime to introduce editor-only types.

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
JzServiceContainer::Provide<JzAssetManager>(assetManager);

// Consume service
auto& assetMgr = JzServiceContainer::Get<JzAssetManager>();
```

### Factory Pattern (Assets)

```cpp
// Register factories
assetManager.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());

// Load asset (sync)
auto textureHandle = assetManager.LoadSync<JzTexture>("textures/player.png");
auto* texture = assetManager.Get(textureHandle);

// Load asset (async)
assetManager.LoadAsync<JzTexture>("textures/player.png", [](JzTextureHandle handle, Bool success) {
    // Callback when loaded
});
```

### Entity-Component-System

```cpp
// Create entity and add components
JzWorld world;
auto entity = world.CreateEntity();
world.AddComponent<JzTransformComponent>(entity);
world.AddComponent<JzMeshComponent>(entity);

// Query and process
auto view = world.View<JzTransformComponent, JzVelocityComponent>();
for (auto [entity, transform, velocity] : view.each()) {
    transform.position += velocity.velocity * deltaTime;
}

// Store singleton services in world context
world.SetContext<JzEventSystem>(std::make_unique<JzEventSystem>());
auto& eventSystem = world.GetContext<JzEventSystem>();
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
| `JzREEditor`        | Executable        | Main application         |

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
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzAssetSystem.h"
#include "JzRE/Runtime/Function/Event/JzEventSystem.h"

// Editor modules
#include "JzRE/Editor/JzEditorUI.h"
```

---

## Future Roadmap

### Near-term

- [ ] Vulkan backend implementation
- [x] Async resource loading (implemented in JzAssetManager)
- [x] Material system improvements (shader variants)
- [x] Shader hot reload (integrated into JzAssetSystem)

### Mid-term

- [ ] Parallel ECS systems
- [ ] Scene serialization
- [ ] Physics integration

### Long-term

- [ ] Multi-threaded command recording
- [ ] Render graph
- [ ] Ray tracing support

📄 See: [Threading Roadmap](threading.md) for detailed evolution plan.
