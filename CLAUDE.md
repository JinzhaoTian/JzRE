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

To disable tests: `cmake -B build -DBUILD_TESTS=OFF`

## Architecture Overview

JzRE is a C++20 cross-platform render engine with a layered Runtime + Editor architecture. Dependencies flow strictly downward:

```
App (JzRE executable)
  └── Editor (JzEditor)
        └── JzRERuntime (interface library linking all layers)
              └── JzREInterface (runtime application framework)
                    └── Function Layer (ECS, Rendering, Scene, Input, Window)
                          └── Resource Layer (ResourceManager, asset factories)
                                └── Platform Layer (RHI abstraction, OpenGL/Vulkan backends, OS APIs)
                                      └── Core Layer (types, math, threading, events, logging)
```

### Key Libraries

| Target | Description |
|--------|-------------|
| `JzRuntimeCore` | Math (JzVector, JzMatrix), types (JzRETypes), threading (JzThreadPool), logging (JzLogger) |
| `JzRuntimePlatform` | RHI command pattern (`JzRHICommandList`, `JzDevice`), GPU objects, platform file dialogs |
| `JzRuntimeResource` | `JzResourceManager`, factories for Texture/Mesh/Model/Shader/Material/Font |
| `JzRuntimeFunction` | `JzRHIRenderer`, `JzScene`, `JzEntityManager`, `JzInputManager`, `JzWindow` |
| `JzREInterface` | Runtime application base class (`JzRERuntime`) with virtual `OnStart/OnUpdate/OnStop` hooks |
| `JzEditor` | ImGui-based editor panels, UI widget wrappers |

### Include Path Convention

```cpp
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzResourceManager.h"
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
