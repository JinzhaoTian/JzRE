# JzRE

A cross-platform, multi-graphics-API game engine built with modern C++20.

## Features

- **Layered Runtime Architecture**: Core, Platform, Resource, Function, and Interface layers
- **RHI Abstraction**: OpenGL and Vulkan runtime selection (with fallback)
- **ECS**: Entity-Component-System powered by EnTT
- **Resource Management**: Automatic caching with reference-counted unloading
- **Mainline CLI Entry**: `JzRE` executable in `src/CLI` for project/asset/shader/scene/run workflows
- **Legacy Editor Example**: Standalone ImGui-based editor under `examples/EditorExample/`

## Build (Root Aggregate)

### Prerequisites

- CMake 3.20+
- C++20 compatible compiler
- vcpkg

### Configure

```bash
cmake --preset macos-clang-static
```

### Build

```bash
cmake --build build
```

### Run CLI

```bash
./build/JzRE/JzRE --help
```

### Testing

```bash
cd build && ctest --output-on-failure
```

To disable tests: `cmake -B build -DJzRE_BUILD_TESTS=OFF`

## Standalone Builds

### CLI (Root Target)

```bash
cmake --preset macos-clang-static
cmake --build build --target JzRE
./build/JzRE/JzRE --help
```

### RuntimeExample (Independent)

```bash
cmake -S examples/RuntimeExample -B examples/RuntimeExample/build
cmake --build examples/RuntimeExample/build
./examples/RuntimeExample/build/RuntimeExample/RuntimeExample --input ./examples/RuntimeExample/build/RuntimeExample/EngineContent/Models/crate.obj
```

Disable RuntimeExample asset sync if needed:
`-DJzRE_RUNTIME_EXAMPLE_SYNC_ENGINE_ASSETS=OFF`

### EditorExample (Legacy, Independent)

```bash
cmake -S examples/EditorExample -B examples/EditorExample/build
cmake --build examples/EditorExample/build
./examples/EditorExample/build/EditorExample/EditorExample
```

## CLI Command Set (v1)

- `JzRE project create|validate|info|set`
- `JzRE asset import|import-model|export`
- `JzRE shader cook|cook-project`
- `JzRE scene validate|stats`
- `JzRE run --project <file.jzreproject>`

Global options:

- `--help`
- `--version`
- `--format text|json`
- `--log-level trace|debug|info|warn|error`

See [docs/architecture/cli.md](docs/architecture/cli.md) for full syntax and exit codes.

## Architecture

```text
Mainline Build (root CMake)
  └── JzRE (CLI)
        └── JzRERuntime
              ├── Function Layer
              ├── Resource Layer
              ├── Platform Layer
              └── Core Layer

EditorExample (legacy standalone build)
  └── JzEditor + JzREEditor
        └── JzRERuntime

RuntimeExample (standalone build)
  └── JzRERuntime
```

For detailed architecture docs, see [docs/architecture/overview.md](docs/architecture/overview.md).

## Documentation

| Document                                               | Description                             |
| ------------------------------------------------------ | --------------------------------------- |
| [Architecture Overview](docs/architecture/overview.md) | Project design and major entry points   |
| [Module Structure](docs/architecture/module.md)        | Module and CMake layout                 |
| [CLI Architecture](docs/architecture/cli.md)           | CLI domains, routing, and exit codes    |
| [RHI Design](docs/architecture/rhi.md)                 | Render Hardware Interface specification |
| [ECS Integration](docs/architecture/ecs.md)            | ECS usage guide                         |
| [Resource Layer](docs/architecture/resource.md)        | Asset management system                 |
| [Threading Roadmap](docs/architecture/threading.md)    | Multi-threading evolution plan          |

## Dependencies

Mainline dependencies are managed via `vcpkg.json` in repository root:

- `glfw3`
- `glad`
- `vulkan`
- `shaderc`
- `spirv-reflect`
- `assimp`
- `entt`
- `spdlog`
- `freetype`

`examples/EditorExample/vcpkg.json` adds legacy editor-only dependencies (notably `imgui`).

## Shader Workflow

- Runtime engine shader source manifests live in `src/EngineContent/ShaderSource/`.
- Legacy editor-only shader source manifests live in `examples/EditorExample/EditorContent/ShaderSource/`.
- Runtime consumes cooked files only: `.jzshader` + `.jzsblob`.
- Root build cooks runtime engine shaders into `build/JzRE/EngineContent/Shaders/` (target: `JzRECookEngineShaders`).
- Legacy EditorExample standalone build cooks both runtime engine shaders and editor-only shaders into its own `EngineContent/Shaders/` output.
- Project override convention:
  - source: `<ProjectRoot>/Content/Shaders/src/`
  - cooked: `<ProjectRoot>/Content/Shaders/`

Manual cook example:

```bash
./build/JzRE/JzREShaderTool \
  --input src/EngineContent/ShaderSource/standard.jzshader.src.json \
  --output-dir build/JzRE/EngineContent/Shaders
```

## Release Packaging

GitHub Actions release workflow (`.github/workflows/release.yml`) produces:

- `JzRE-runtime-<os>-<arch>-<tag>.zip`
- `JzRE-dev-<os>-<arch>-<tag>.zip`

`runtime` archives contain cooked shader artifacts only.
`dev` archives include runtime payload + shader source + `JzREShaderTool`.

## License

JzRE is licensed under the MIT License. See [LICENSE](LICENSE) for details.
