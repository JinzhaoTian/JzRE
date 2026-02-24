# JzRE Architecture Overview

## Introduction

JzRE is a cross-platform game engine built with C++20.

Architecture is split into a mainline Runtime and a standalone Editor example:

- Runtime (`src/Runtime/**`) provides engine capabilities and is built from the root project.
- Editor example (`examples/EditorExample/**`) consumes runtime interfaces in an independent CMake project.

Dependency direction is strictly top-down from editor integration to Core.

## Layered Architecture

```text
Mainline Build (root CMakeLists.txt)
  -> JzRERuntime (runtime integration surface)
    -> JzRuntimeFunction (ECS, systems)
      -> JzRuntimeResource (assets/factories/cache)
        -> JzRuntimePlatform (RHI, window, backend)
          -> JzRuntimeCore (types/math/logging/threading)

Standalone Example Build (examples/EditorExample/CMakeLists.txt)
  -> EditorExample (Executable target)
    -> JzEditor (Editor UI/tools)
      -> JzRERuntime (same runtime layers as above)
```

## Design Principles

| Principle                           | Description                                                                                         |
| ----------------------------------- | --------------------------------------------------------------------------------------------------- |
| Separation of concerns              | Each module owns a focused responsibility.                                                          |
| Strict dependency flow              | Upper layers depend on lower layers only.                                                           |
| ECS-centric runtime                 | Runtime frame behavior is driven by `JzWorld` systems.                                              |
| Service locator for shared services | Runtime services are exposed via `JzServiceContainer`.                                              |
| Command-list rendering API          | Runtime/editor rendering is recorded through `JzRHICommandList` and executed by backend devices.     |
| Offline shader cooking              | Shader authoring uses HLSL; runtime consumes cooked `.jzshader` + `.jzsblob` artifacts only.         |

## Runtime and Editor Boundary

Runtime must stay editor-agnostic:

1. Runtime public API must not expose panel-specific editor concepts.
2. Runtime must not depend on editor UI/tooling libraries.
3. Editor-specific rendering behavior must be injected via runtime extension points (render targets, graph contributions, callbacks).

## Core Layer

Key responsibilities:

- fundamental types and math (`JzRETypes`, vectors, matrices)
- timing and utilities (`JzClock`)
- threading primitives (`JzThreadPool`, `JzTaskQueue`)
- logging (`spdlog` integration)
- service container (`JzServiceContainer`)

## Platform Layer

Key responsibilities:

- window abstraction (`JzIWindowBackend`, GLFW backend)
- RHI abstraction (`JzGraphicsContext`, `JzDevice`)
- backend resources (pipeline/buffer/texture/VAO/framebuffer)

Current backend status:

- OpenGL: implemented and kept as compatibility backend
- Vulkan: implemented (runtime/editor path available, automatic fallback to OpenGL)
- D3D12: API enum + project serialization + cooked payload target added (native backend integration pending)
- Metal: API enum + project serialization + cooked payload target added (native backend integration pending)

Notes on rendering command path:

- runtime/editor render flow records commands through `JzRHICommandList`
- backend devices execute recorded lists via `ExecuteCommandList(...)`

## Resource Layer

Key responsibilities:

- `JzAssetManager` lifecycle and registries
- type factories (`JzShaderFactory`, `JzModelFactory`, ...)
- sync/async loading
- search-path based asset resolution
- ECS-facing integration via `JzAssetSystem`
- cooked shader package loading (`.jzshader` manifest + `.jzsblob` blob)

## Shader Pipeline (Current)

The shader pipeline is now offline-first:

1. Author HLSL source + source manifest (`*.jzshader.src.json`) in `resources/shaders/src/`.
2. Run `JzREShaderTool` to produce runtime artifacts:
   - `*.jzshader` (manifest)
   - `*.jzsblob` (binary chunk blob)
3. Runtime loads `JzShader` from cooked manifest and selects variant by `keywordMask`.
4. `JzShaderProgramDesc` carries cooked payload format (`GLSL`, `SPIRV`, `DXIL`, `MSL`) and bytes to RHI backends.

## Function Layer

Key responsibilities:

- ECS world and systems (`JzWorld`, `JzSystem`)
- window/input/event/asset/camera/light/render systems
- render orchestration (`JzRenderSystem`, `JzRenderGraph`, `JzRenderOutput`)
- render pass execution context (`JzRGPassContext`) for target/framebuffer binding

Current runtime system registration order:

1. `JzWindowSystem`
2. `JzInputSystem`
3. `JzEventSystem`
4. `JzAssetSystem`
5. `JzCameraSystem`
6. `JzLightSystem`
7. `JzRenderSystem`

`JzWorld::Update()` executes systems in this order.

## Runtime Frame Flow

Current `JzRERuntime::Run()` flow (simplified):

1. poll window events (`m_windowSystem->PollWindowEvents()`)
2. `OnUpdate(delta)`
3. `m_graphicsContext->BeginFrame()`
4. `m_world->Update(delta)`
5. `OnRender(delta)`
6. `m_graphicsContext->EndFrame()`
7. clear input frame state
8. `m_graphicsContext->Present()`

`Present()` behavior:
- OpenGL: `Finish()` + `SwapBuffers()`
- Vulkan: device-side submit + present (swapchain path)

## Render Target Lifecycle

RenderTarget/RenderOutput creation follows a two-phase model:

1. **Record creation (construction time)**: `JzRenderTarget` record and `JzRenderOutput` object are created. No GPU resources are allocated. This happens in `JzRenderSystem` constructor (default target) and `JzView` constructor (panel targets).
2. **GPU resource allocation (first Update)**: `JzRenderOutput::EnsureSize()` allocates textures and framebuffer when window size becomes available during the first `JzRenderSystem::Update()` call.

The default render target (`DefaultScene`) is created at `JzRenderSystem` construction, uses `getDesiredSize = m_frameSize`, and cannot be unregistered. Panel targets are created at `JzView` construction (including initially-closed panels); `shouldRender` controls whether passes execute.

## Editor Layer

The Editor uses runtime extension points rather than editor-specific runtime internals:

- `JzView` registers logical render targets at construction time.
- Initially-closed panels also register; `shouldRender` controls pass execution.
- panels query `JzRenderOutput` by handle and display textures.
- editor feature drawing is appended as graph contributions
  with scope control (`MainScene` / `RegisteredTarget` / `All`).
  - `MainScene` = default target only, `RegisteredTarget` = non-default targets only.
  Contributions can read pass execution metadata through `JzRenderGraphContributionContext::passContext`.

## Key Patterns

### Service Locator

```cpp
JzServiceContainer::Provide<JzAssetManager>(assetManager);
auto &mgr = JzServiceContainer::Get<JzAssetManager>();
```

### Asset Factory

```cpp
assetSystem.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
auto handle = assetSystem.LoadSync<JzTexture>("textures/albedo.png");
```

### ECS Query

```cpp
auto view = world.View<JzTransformComponent, JzMeshAssetComponent>();
for (auto [entity, transform, meshAsset] : view.each()) {
    // system logic
}
```

### Command List Rendering Path (Current)

```cpp
auto cmd = device.CreateCommandList("Pass");
cmd->Begin();
cmd->BindPipeline(pipeline);
cmd->BindVertexArray(vertexArray);
cmd->DrawIndexed(drawParams);
cmd->End();
device.ExecuteCommandList(cmd);
```

## Documentation Map

- [layers.md](layers.md)
- [module.md](module.md)
- [ecs.md](ecs.md)
- [rendering_pipeline.md](rendering_pipeline.md)
- [rhi.md](rhi.md)
- [resource.md](resource.md)
- [asset_system.md](asset_system.md)
- [threading.md](threading.md)

## Source References

- `src/Runtime/Interface/src/JzRERuntime.cpp`
- `src/Runtime/Function/src/ECS/JzWorld.cpp`
- `src/Runtime/Function/src/ECS/JzRenderSystem.cpp`
- `examples/EditorExample/Panels/src/JzView.cpp`
- `examples/EditorExample/Application/src/JzREEditor.cpp`
