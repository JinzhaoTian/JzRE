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
│   ├── Core/         # Fundamental types, math, threading, logging
│   ├── RHI/          # Render Hardware Interface (abstract)
│   ├── Graphics/     # Graphics API backends (OpenGL, Vulkan)
│   ├── Resource/     # Asset management system
│   ├── ECS/          # Entity-Component-System
│   ├── Platform/     # OS-specific abstractions
│   ├── UI/           # ImGui wrapper components
│   ├── Editor/       # Editor application logic
│   └── App/          # Application entry points
├── docs/             # Architecture documentation
├── tests/            # Unit tests
├── resources/        # Runtime assets (shaders, textures)
└── programs/         # Example programs
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
JzServiceContainer::Provide<JzResourceManager>(resourceManager);

// Retrieve
auto& resMgr = JzServiceContainer::Get<JzResourceManager>();
```

### Factory Pattern (Resources)

```cpp
resourceManager.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
auto texture = resourceManager.GetResource<JzTexture>("path/to/texture.png");
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

### ECS Pattern

```cpp
// Create entity
auto entity = entityManager.CreateEntity();

// Add components
entityManager.AddComponent<JzTransformComponent>(entity, position, rotation, scale);
entityManager.AddComponent<JzMeshComponent>(entity, mesh);

// Query entities
for (auto e : entityManager.View<JzTransformComponent, JzMeshComponent>()) {
    auto& transform = entityManager.GetComponent<JzTransformComponent>(e);
    // ...
}
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
App → Editor → UI, ECS, Resource
ECS → RHI, Resource, Core
Resource → RHI, Core
UI → RHI, Core
RHI → Core
Graphics → RHI, Platform
Platform → Core
```

**Rule**: Higher layers depend on lower layers. Never introduce reverse dependencies.

---

## Current Implementation Status

| Module | Status | Notes |
|--------|--------|-------|
| Core | ✅ Complete | ThreadPool, TaskQueue, Events, Math |
| RHI | ✅ Complete | Abstract device, command lists |
| OpenGL | ✅ Complete | Full implementation |
| Vulkan | 🚧 Planned | Architecture ready, no implementation |
| Resource | ✅ Complete | Sync loading, weak_ptr cache |
| ECS | ✅ Complete | ComponentPool, Systems |
| Platform | ✅ Complete | File dialogs, message boxes |
| UI | ✅ Complete | ImGui wrappers |
| Editor | ✅ Complete | Panels, views, canvas |

---

## Common Tasks

### Adding a New Resource Type

1. Create `JzNewResource.h` in `Resource/include/JzRE/Resource/`
2. Inherit from `JzResource`, implement `Load()` and `Unload()`
3. Create `JzNewResourceFactory.h` inheriting `JzResourceFactory`
4. Register factory in initialization:
   ```cpp
   resourceManager.RegisterFactory<JzNewResource>(std::make_unique<JzNewResourceFactory>());
   ```

### Adding a New RHI Command

1. Add enum to `JzRHIECommandType` in `JzRHICommand.h`
2. Create `JzRHINewCommand.h` inheriting `JzRHICommand`
3. Add recording method to `JzRHICommandList`
4. Implement `Execute()` in each graphics backend

### Adding a New ECS Component

1. Define struct in `ECS/include/JzRE/ECS/JzComponent.h`
2. No registration needed - `JzComponentPool<T>` is template-based
3. Use via `entityManager.AddComponent<NewComponent>(entity, ...)`

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
| `JzResourceManager` | ✅ Cache access mutex-protected |
| `JzOpenGLDevice` | ❌ Single-threaded only |
| `JzEntityManager` | ❌ Not thread-safe, use single thread |

---

## Don'ts

- ❌ Don't call OpenGL directly outside `Graphics/OpenGL/`
- ❌ Don't create circular dependencies between modules
- ❌ Don't use raw `new`/`delete` - use smart pointers
- ❌ Don't use Chinese in code comments (English only)
- ❌ Don't modify `Core` module to depend on higher layers
- ❌ Don't bypass `JzResourceManager` for asset loading

---

## Documentation References

| Document | Purpose |
|----------|---------|
| [architecture.md](docs/architecture.md) | Overall layered design |
| [rhi.md](docs/rhi.md) | RHI abstraction details |
| [resource_layer_design.md](docs/resource_layer_design.md) | Resource system |
| [threading_roadmap.md](docs/threading_roadmap.md) | Multithreading evolution |
| [module.md](docs/module.md) | CMake and directory structure |
| [design.md](docs/design.md) | Class and sequence diagrams |
| [GEMINI.md](GEMINI.md) | Project configuration for AI agents |

---

## Contact

- **Author**: Jinzhao Tian
- **Repository**: JzRE
