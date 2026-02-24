# JzRE CLI Architecture

## Overview

`JzRE` is the command-line entrypoint for core engine operations. The CLI lives in `src/CLI` and is enabled in root aggregate build through `JzRE_BUILD_CLI=ON`.

The first release freezes the domain set to:

- `project`
- `asset`
- `shader`
- `scene`
- `run`

## Command Surface

### Project

```bash
JzRE project create --name <name> --dir <dir> [--render-api auto|opengl|vulkan|d3d12|metal] [--startup-mode runtime|host]
JzRE project validate --project <file.jzreproject>
JzRE project info --project <file.jzreproject> [--format text|json]
JzRE project set --project <file.jzreproject> --default-scene <path> [--render-api ...] [--shader-auto-cook on|off] [--startup-mode runtime|host]
```

### Asset

```bash
JzRE asset import --project <file.jzreproject> --src <file...> [--overwrite] [--subfolder <ContentSubDir>]
JzRE asset import-model --project <file.jzreproject> --src <model.obj|fbx> [--overwrite]
JzRE asset export --project <file.jzreproject> --src <file...> --out <dir> [--overwrite] [--flat]
```

### Shader

```bash
JzRE shader cook --input <manifest-or-dir> --output-dir <dir> [--tool <path-to-JzREShaderTool>]
JzRE shader cook-project --project <file.jzreproject> [--tool <path-to-JzREShaderTool>]
```

Notes:

- `shader` commands reuse `JzREShaderTool` by process invocation.
- CLI does not duplicate shader compiler implementation.

### Scene

```bash
JzRE scene validate --file <scene.jzscene>
JzRE scene stats --file <scene.jzscene> [--format text|json]
```

### Run

```bash
JzRE run --project <file.jzreproject> [--rhi auto|opengl|vulkan] [--width <n>] [--height <n>] [--title <name>]
```

`run` uses a minimal runtime shell class derived from `JzRERuntime` and does not depend on `RuntimeExample` logic.

## Global Options

```bash
--help
--version
--format text|json
--log-level trace|debug|info|warn|error
```

Global options are parsed before domain dispatch.

## Exit Codes

| Code | Meaning |
| --- | --- |
| `0` | Success |
| `2` | Invalid arguments |
| `3` | File/path I/O error |
| `4` | Project load/configuration error |
| `5` | External tool execution failure |
| `6` | Runtime launch failure |

## Internal Design

The CLI is organized as:

- `JzCliTypes`: exit codes, output format, command result
- `JzCliArgParser`: hand-written parser (no third-party parser dependency)
- `JzCliCommandRegistry`: domain-to-handler registration and routing
- `JzCliContext`: runtime service initialization and shared runtime-facing services
- `commands/*`: domain handlers (`project`, `asset`, `shader`, `scene`, `run`)

## CMake Integration

### Module CMake (`src/CLI/CMakeLists.txt`)

`src/CLI/CMakeLists.txt` defines CLI targets from local sources:

- `JzCLI` (object library)
- `JzRE` (executable target, output name: `JzRE`)

### Test CMake (`tests/CLI/CMakeLists.txt`)

`tests/CLI/CMakeLists.txt` defines:

- `TESTJzRECLI` (when `JzRE_CLI_BUILD_TESTS=ON` and `JzCLI` target exists)

### Root + Source Aggregation

Root `CMakeLists.txt` uses:

- `option(JzRE_BUILD_CLI "Build JzRE command line interface" ON)`
- `add_subdirectory(src)`

`src/CMakeLists.txt` then conditionally adds `add_subdirectory(CLI)` when `JzRE_BUILD_CLI=ON`.

The CLI has no CMake dependency on `examples/EditorExample` or `examples/RuntimeExample`.

## Examples

```bash
JzRE --version
JzRE project create --name Demo --dir ./Demo --render-api vulkan --startup-mode runtime
JzRE project validate --project ./Demo/Demo.jzreproject
JzRE shader cook-project --project ./Demo/Demo.jzreproject
JzRE run --project ./Demo/Demo.jzreproject --rhi auto --width 1600 --height 900
```
