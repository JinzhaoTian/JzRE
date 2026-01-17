# JzRE

A cross-platform, multi-graphics-API game engine built with modern C++20.

## Features

- **Layered Architecture**: Clean separation between Core, Platform, Resource, Function, and Editor layers
- **RHI Abstraction**: Graphics API abstraction supporting OpenGL (Vulkan planned)
- **ECS**: Entity-Component-System using EnTT library
- **Resource Management**: Automatic caching with reference-counted unloading
- **ImGui Editor**: Built-in development tools with docking support

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

4. **Run**
```bash
./build/JzRE/JzRE
```

### Testing

```bash
cd build && ctest --output-on-failure
```

To disable tests: `cmake -B build -DBUILD_TESTS=OFF`

## Architecture

```
App (JzRE executable)
  └── Editor (JzEditor)
        └── Runtime
              ├── Function Layer (ECS, Rendering, Scene, Input, Window)
              ├── Resource Layer (ResourceManager, Asset Factories)
              ├── Platform Layer (RHI, Graphics Backends, OS APIs)
              └── Core Layer (Types, Math, Threading, Events, Logging)
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

Managed via vcpkg:

- **glfw3** - Window and input
- **glad** - OpenGL loader
- **imgui** - Editor UI (docking branch)
- **assimp** - 3D model loading
- **entt** - ECS library
- **spdlog** - Logging
- **freetype** - Font rendering

## License

See [LICENSE](LICENSE) for details.
