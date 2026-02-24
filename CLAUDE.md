# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (uses vcpkg for dependencies)
cmake -B build

# Build all targets
cmake --build build

# Run tests
cd build && ctest --output-on-failure

# Run a single test executable
./build/JzRE/TESTJzRECore

# Run main application
./build/JzRE/JzRE
```

To disable tests: `cmake -B build -DJzRE_BUILD_TESTS=OFF`

## Architecture Overview

JzRE is a C++20 cross-platform render engine with a layered Runtime + Editor architecture. Dependencies flow strictly downward:

```
App (JzRE executable)
  └── Editor (JzEditor)
        └── JzRERuntime (interface library linking all layers)
              └── JzREInterface (runtime application framework)
                    └── Function Layer (ECS, Rendering, Scene, Input, Window)
                          └── Resource Layer (JzAssetManager, asset factories)
                                └── Platform Layer (RHI abstraction, OpenGL/Vulkan backends, OS APIs)
                                      └── Core Layer (types, math, threading, events, logging)
```

### Key Libraries

| Target              | Description                                                                                           |
| ------------------- | ----------------------------------------------------------------------------------------------------- |
| `JzRuntimeCore`     | Math (JzVector, JzMatrix), types (JzRETypes), threading (JzThreadPool), logging (JzLogger)            |
| `JzRuntimePlatform` | RHI command pattern (`JzRHICommandList`, `JzDevice`), GPU objects, platform file dialogs              |
| `JzRuntimeResource` | `JzAssetManager`, factories for Texture/Mesh/Model/Shader/Material/Font                               |
| `JzRuntimeFunction` | ECS systems (`JzRenderSystem`, `JzCameraSystem`, `JzLightSystem`, `JzWindowSystem`), `JzInputManager` |
| `JzREInterface`     | Runtime application base class (`JzRERuntime`) with virtual `OnStart/OnUpdate/OnStop` hooks           |
| `JzEditor`          | ImGui-based editor panels, UI widget wrappers                                                         |

### Include Path Convention

```cpp
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"
#include "JzRE/Runtime/Platform/Threading/JzRenderThreadPool.h"
#include "JzRE/Runtime/Platform/Window/IWindowBackend.h"
#include "JzRE/Runtime/Platform/Dialog/JzFileDialog.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Function/ECS/JzEntityManager.h"
#include "JzRE/Runtime/JzRERuntime.h"  // Runtime application base class
#include "JzRE/Editor/JzEditor.h"
```

### Design Patterns

- **Command Pattern for RHI**: Commands are recorded to `JzRHICommandList` then executed via `JzDevice`
- **Service Container**: Use `JzServiceContainer::Provide<T>()` and `JzServiceContainer::Get<T>()` for DI
- **ECS**: EnTT-based with systems in `src/Runtime/Function/ECS/`

## Code Organization

- `src/Runtime/` - Engine runtime layers (Core, Platform, Resource, Function)
- `src/Editor/` - ImGui-based editor tools and UI wrappers
- `src/App/` - Application entry points (JzRERuntime, JzREHub, main.cpp)
- `tests/` - GTest-based tests, organized by module (e.g., `tests/Core/`)
- `examples/` - Example applications using the runtime
- `programs/JzREHeaderTool/` - Code generation tool using libclang for reflection/serialization

## Testing

Tests use Google Test. Test files follow the pattern `Test*.cpp` in module subdirectories:

- `tests/Core/TestJzMatrix.cpp`, `TestJzVector.cpp`, `TestJzClock.cpp`, `TestJzRETypes.cpp`

The test executable `TESTJzRECore` links against `JzRuntimeCore` and `GTest::gtest_main`.

## External Dependencies (vcpkg)

glfw3, glad, imgui (with docking), assimp, stb, gtest, nlohmann-json, spdlog, fmt, freetype, entt

## Documentation Requirements

**IMPORTANT**: When making changes to the engine architecture, you MUST update the corresponding documentation in `/docs/architecture/`:

| Document                | Content                                                            |
| ----------------------- | ------------------------------------------------------------------ |
| `overview.md`           | High-level engine architecture and design philosophy               |
| `layers.md`             | Layer dependency hierarchy (Core → Platform → Resource → Function) |
| `module.md`             | Module structure and inter-module dependencies                     |
| `ecs.md`                | ECS components, systems, and entity management                     |
| `rendering_pipeline.md` | Rendering flow and system update order                             |
| `rhi.md`                | RHI abstraction and command pattern                                |
| `resource.md`           | Resource management and asset factories                            |
| `threading.md`          | Threading model and thread pool usage                              |

Architecture changes that require documentation updates include:

- Adding/removing/modifying ECS systems or components
- Changing the rendering pipeline or system execution order
- Modifying core abstractions (JzRERuntime, JzWorld, etc.)
- Adding new subsystems to any layer
- Changing layer dependencies or module structure
- Modifying RHI commands or resource loading patterns

**Always keep documentation in sync with code changes.**
