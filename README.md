# JzRE

A cross-platform, multi-graphics-API game engine built with modern C++20.

## Features

- **Layered Runtime Architecture**: Clean separation between Core, Platform, Resource, Function, and Interface layers
- **RHI Abstraction**: Graphics API abstraction supporting OpenGL + Vulkan (Auto-select + fallback)
- **ECS**: Entity-Component-System using EnTT library
- **Resource Management**: Automatic caching with reference-counted unloading
- **Standalone Editor Example**: ImGui-based editor app maintained under `examples/EditorExample/`

## Build

### Prerequisites

- CMake 3.20+
- C++20 compatible compiler
- vcpkg (dependencies managed automatically)

### Steps

1. **Clone**
```bash
git clone https://github.com/jinzhaotian/JzRE.git
cd JzRE
```

2. **Configure**
```bash
cmake -B build
```

3. **Build**
```bash
cmake --build build
```

4. **Run Runtime Example**
```bash
./build/JzRE/RuntimeExample --input ./build/JzRE/models/crate.obj
```

### Testing

```bash
cd build && ctest --output-on-failure
```

To disable tests: `cmake -B build -DBUILD_TESTS=OFF`

## Standalone Editor Example

`EditorExample` is intentionally separated from the root build graph.

1. **Configure**
```bash
cmake -S examples/EditorExample -B examples/EditorExample/build
```

2. **Build**
```bash
cmake --build examples/EditorExample/build
```

3. **Run**
```bash
./examples/EditorExample/build/EditorExample/EditorExample
```

## Architecture

```
Runtime Applications (mainline build)
  └── JzRERuntime
        ├── Function Layer (ECS, Rendering, Scene, Input, Window)
        ├── Resource Layer (ResourceManager, Asset Factories)
        ├── Platform Layer (RHI, Graphics Backends, OS APIs)
        └── Core Layer (Types, Math, Threading, Events, Logging)

EditorExample (standalone example build)
  └── JzEditor + JzREEditor
        └── JzRERuntime (same runtime layers as above)
```

For detailed architecture documentation, see [docs/architecture/overview.md](docs/architecture/overview.md).

## Documentation

| Document | Description |
|----------|-------------|
| [Architecture Overview](docs/architecture/overview.md) | Project design and layer descriptions |
| [RHI Design](docs/architecture/rhi.md) | Render Hardware Interface specification |
| [ECS Integration](docs/architecture/ecs.md) | Entity-Component-System usage guide |
| [Resource Layer](docs/architecture/resource.md) | Asset management system |
| [Threading Roadmap](docs/architecture/threading.md) | Multi-threading evolution plan |

## Dependencies

Mainline dependencies are managed via `vcpkg.json` in repository root:

- **glfw3** - Window and input
- **glad** - OpenGL loader
- **vulkan** - Vulkan loader and headers
- **shaderc** - GLSL to SPIR-V compiler
- **spirv-reflect** - SPIR-V reflection
- **assimp** - 3D model loading
- **entt** - ECS library
- **spdlog** - Logging
- **freetype** - Font rendering

`examples/EditorExample/vcpkg.json` adds editor-only dependencies (notably `imgui`).

## License

JzRE is licensed under the MIT License. See [LICENSE](LICENSE) for details.
