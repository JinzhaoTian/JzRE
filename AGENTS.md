# AGENTS.md - AI Agent Guidelines for JzRE

## Project Overview

**JzRE** is a cross-platform, multi-graphics-API game engine written in C++20. The engine features a modular architecture with an abstracted Rendering Hardware Interface (RHI), Entity-Component-System (ECS) for scene management, and an ImGui-based editor.

---

## Quick Reference

| Aspect | Details |
|--------|---------|
| Language | C++20 |
| Build System | CMake 3.20+ |
| Package Manager | vcpkg |
| Graphics API | OpenGL 3.3+ (Vulkan planned) |
| UI Framework | ImGui (wrapped in stateful classes) |
| Platforms | Windows, macOS, Linux |

---

## Project Structure

```
JzRE/
├── src/
│   ├── Runtime/
│   │   ├── Core/         # Fundamental types, math, threading, logging
│   │   ├── Platform/     # RHI abstraction, OpenGL/Vulkan backends, OS APIs
│   │   ├── Resource/     # Asset management (JzAssetManager, factories)
│   │   ├── Function/     # ECS systems, input, window, events
│   │   └── Interface/    # JzRERuntime application framework
│   └── Editor/           # ImGui-based editor panels and UI
├── docs/                 # Architecture documentation
├── tests/                # Unit tests (GTest)
├── resources/            # Runtime assets (shaders, textures)
└── programs/             # Code generation tools (JzREHeaderTool)
```

---

## Coding Conventions

### Naming

| Type | Pattern | Example |
|------|---------|---------|
| Class | `Jz` prefix + PascalCase | `JzResourceManager` |
| Enum | `JzE` prefix | `JzERHIType`, `JzEBufferType` |
| Member variable | `m_` prefix + camelCase | `m_resourceCache` |
| Static variable | `__` prefix + UPPER_CASE | `__SERVICES` |
| Method | PascalCase | `GetResource()` |
| File | Match class name | `JzResourceManager.h/.cpp` |

### Header Organization

```
Module/
├── include/JzRE/Module/
│   └── JzPublicClass.h    # Public headers
└── src/
    ├── JzPublicClass.cpp
    └── JzInternal.h       # Private headers
```

### Include Style

```cpp
// Cross-module: angle brackets with full path
#include <JzRE/Core/JzLogger.h>
#include <JzRE/RHI/JzDevice.h>

// Same module: quotes
#include "JzInternalHelper.h"
```

---

## Documentation Requirements

All code must follow **Doxygen** format in **English**:

```cpp
/**
 * @brief Short description.
 *
 * @param paramName Description of parameter.
 *
 * @return Description of return value.
 *
 * @throw ExceptionType When this is thrown.
 */
```

File headers must include:
```cpp
/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */
```

---

## Key Patterns

### Service Locator

```cpp
// Register
JzServiceContainer::Provide<JzAssetManager>(assetManager);

// Retrieve
auto& assetMgr = JzServiceContainer::Get<JzAssetManager>();
```

### Factory Pattern (Assets)

```cpp
assetManager.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
auto textureHandle = assetManager.LoadSync<JzTexture>("path/to/texture.png");
auto* texture = assetManager.Get(textureHandle);
```

### Command Pattern (RHI)

```cpp
auto cmdList = device->CreateCommandList();
cmdList->Begin();
cmdList->BindPipeline(pipeline);
cmdList->DrawIndexed(params);
cmdList->End();
device->ExecuteCommandList(cmdList);
```

### ECS Pattern (EnTT-based)

```cpp
JzWorld world;

// Create entity
auto entity = world.CreateEntity();

// Add components
world.AddComponent<JzTransformComponent>(entity);
world.AddComponent<JzMeshComponent>(entity);

// Query entities
auto view = world.View<JzTransformComponent, JzMeshComponent>();
for (auto [e, transform, mesh] : view.each()) {
    transform.position += velocity * deltaTime;
}

// Store singletons in world context
world.SetContext<JzEventSystem>(std::make_unique<JzEventSystem>());
auto& eventSystem = world.GetContext<JzEventSystem>();
```

---

## Build Commands

### Configure
```bash
# Windows
cmake --preset windows-msvc-static

# macOS
cmake --preset macos-clang-static
```

### Build
```bash
cmake --build build
```

### Run
```bash
./build/JzRE/JzRE
```

---

## Module Dependencies

```
App → Editor → JzRERuntime
JzRERuntime → JzRuntimeFunction → JzRuntimeResource → JzRuntimePlatform → JzRuntimeCore
```

**Detailed:**
```
JzREEditor (Executable)
  └── JzEditor (Static) → JzRERuntime, imgui
        └── JzRERuntime (Interface) → All runtime layers
              └── JzRuntimeFunction (Static) → ECS, Event, Input, Window systems
                    └── JzRuntimeResource (Static) → JzAssetManager, factories
                          └── JzRuntimePlatform (Static) → RHI, OpenGL, Window
                                └── JzRuntimeCore (Static) → Types, math, logging
```

**Rule**: Higher layers depend on lower layers. Never introduce reverse dependencies.

---

## Current Implementation Status

| Module | Status | Notes |
|--------|--------|-------|
| JzRuntimeCore | ✅ Complete | ThreadPool, TaskQueue, Math, Logging, Clock |
| JzRuntimePlatform | ✅ Complete | RHI abstraction, OpenGL backend, Window backend |
| OpenGL Backend | ✅ Complete | Full implementation |
| Vulkan Backend | 🚧 Planned | Architecture ready, no implementation |
| JzRuntimeResource | ✅ Complete | JzAssetManager, async loading, LRU cache, hot reload |
| JzRuntimeFunction | ✅ Complete | ECS (EnTT), Systems, Event system |
| JzREInterface | ✅ Complete | JzRERuntime application framework |
| JzEditor | ✅ Complete | Panels, views, canvas, 40+ UI widgets |

---

## Common Tasks

### Adding a New Resource Type

1. Create `JzNewResource.h` in `src/Runtime/Resource/include/JzRE/Runtime/Resource/`
2. Inherit from `JzResource`, implement `Load()` and `Unload()`
3. Create `JzNewResourceFactory.h` inheriting `JzResourceFactory`
4. Register factory in `JzAssetSystem::Initialize()`:
   ```cpp
   assetManager->RegisterFactory<JzNewResource>(std::make_unique<JzNewResourceFactory>());
   ```

### Adding a New RHI Command

1. Add enum to `JzRHIECommandType` in `JzRHICommand.h`
2. Create `JzRHINewCommand.h` inheriting `JzRHICommand`
3. Add recording method to `JzRHICommandList`
4. Implement `Execute()` in each graphics backend

### Adding a New ECS Component

1. Define struct in `src/Runtime/Function/include/JzRE/Runtime/Function/ECS/Jz*Components.h`
2. No registration needed - EnTT is template-based
3. Use via `world.AddComponent<NewComponent>(entity)`
4. Query via `world.View<NewComponent, ...>()`

### Adding a New UI Widget

1. Create `JzNewWidget.h` in `UI/include/JzRE/UI/`
2. Inherit from `JzWidget` or `JzWidgetContainer`
3. Implement `_Draw_Impl()` using ImGui calls

### Adding Platform-Specific Code

1. Create interface in `Platform/include/JzRE/Platform/`
2. Implement in platform subdirectories:
   - `Platform/Windows/`
   - `Platform/macOS/`
   - `Platform/Linux/`
3. Use preprocessor guards: `#ifdef _WIN32`, `#elif __APPLE__`, etc.

---

## Threading Considerations

| Component | Thread Safety |
|-----------|---------------|
| `JzThreadPool` | ✅ Thread-safe |
| `JzTaskQueue` | ✅ Thread-safe |
| `JzRHICommandList` | ✅ Recording is thread-safe |
| `JzAssetManager` | ✅ Fine-grained mutexes, async loading supported |
| `JzAssetRegistry` | ✅ Uses shared_mutex for read-heavy workloads |
| `JzOpenGLDevice` | ❌ Single-threaded only |
| `JzWorld` | ❌ Not thread-safe, use single thread |

---

## Don'ts

- ❌ Don't call OpenGL directly outside `src/Runtime/Platform/src/OpenGL/`
- ❌ Don't create circular dependencies between modules
- ❌ Don't use raw `new`/`delete` - use smart pointers
- ❌ Don't use Chinese in code comments (English only)
- ❌ Don't modify `JzRuntimeCore` module to depend on higher layers
- ❌ Don't bypass `JzAssetManager` for asset loading
- ❌ Don't make architectural changes without updating `docs/architecture/`
- ❌ Don't use service container for event system (use `JzWorld::GetContext<JzEventSystem>()` instead)

---

## Documentation Maintenance (MANDATORY)

AI agents **MUST** update documentation when making code changes. This is not optional.

### Auto-Update Rules

| Code Change | Documentation to Update |
|-------------|------------------------|
| New class/module added | `docs/architecture/overview.md`, `docs/architecture/module.md` |
| RHI or rendering changes | `docs/architecture/rhi.md`, `docs/architecture/rendering_pipeline.md` |
| Resource/asset system changes | `docs/architecture/resource.md`, `docs/architecture/asset_system.md` |
| Threading/Worker Thread changes | `docs/architecture/threading.md` |
| Module structure changes | `docs/architecture/module.md`, `docs/architecture/layers.md` |
| ECS components/systems | `docs/architecture/ecs.md` |
| Input system changes | `docs/architecture/input_system_design.md` |

### Documentation Standards

1. **Language**: All documentation must be in English
2. **Format**: Markdown with proper headings, code blocks, and tables
3. **Diagrams**: Include ASCII or Mermaid diagrams for complex concepts
4. **Consistency**: Documentation must match the actual code implementation

### Required Documentation Updates Checklist

Before completing any task, verify:

- [ ] Did I add/modify any classes? → Update `docs/architecture.md`
- [ ] Did I change the RHI layer? → Update `docs/rhi.md`
- [ ] Did I modify threading behavior? → Update `docs/threading_roadmap.md`
- [ ] Did I add new dependencies? → Update `docs/module.md`
- [ ] Did I add new patterns? → Update `docs/design.md`

### Example: Adding a New Renderer Feature

After adding rendering code in `src/Runtime/Function/src/Rendering/`:

1. Update `docs/rhi.md` with new rendering capabilities
2. Update `docs/design.md` if new class relationships were added
3. Update `docs/threading_roadmap.md` if threading model changed
4. Add inline Doxygen comments in the code

---

## Documentation References

| Document | Purpose |
|----------|---------|
| [overview.md](docs/architecture/overview.md) | High-level engine architecture |
| [layers.md](docs/architecture/layers.md) | Layer dependency hierarchy |
| [module.md](docs/architecture/module.md) | CMake and directory structure |
| [ecs.md](docs/architecture/ecs.md) | ECS components and systems |
| [rendering_pipeline.md](docs/architecture/rendering_pipeline.md) | Rendering flow and system order |
| [rhi.md](docs/architecture/rhi.md) | RHI abstraction details |
| [resource.md](docs/architecture/resource.md) | Resource/asset management |
| [asset_system.md](docs/architecture/asset_system.md) | JzAssetManager and hot reload |
| [threading.md](docs/architecture/threading.md) | Threading model |
| [input_system_design.md](docs/architecture/input_system_design.md) | Input handling design |

---

## Contact

- **Author**: Jinzhao Tian
- **Repository**: JzRE
