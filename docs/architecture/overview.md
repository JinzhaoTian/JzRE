# JzRE Architecture Overview

## Introduction

JzRE is a cross-platform game engine built with C++20.

Architecture is split into Runtime and Editor:

- Runtime (`src/Runtime/**`) provides engine capabilities.
- Editor (`src/Editor/**`) consumes runtime interfaces.

Dependency direction is strictly top-down from Editor to Core.

## Layered Architecture

```text
JzREEditor (Executable)
  -> JzEditor (Editor UI/tools)
    -> JzRERuntime (runtime integration surface)
      -> JzRuntimeFunction (ECS, systems)
        -> JzRuntimeResource (assets/factories/cache)
          -> JzRuntimePlatform (RHI, window, backend)
            -> JzRuntimeCore (types/math/logging/threading)
```

## Design Principles

| Principle                           | Description                                                                                         |
| ----------------------------------- | --------------------------------------------------------------------------------------------------- |
| Separation of concerns              | Each module owns a focused responsibility.                                                          |
| Strict dependency flow              | Upper layers depend on lower layers only.                                                           |
| ECS-centric runtime                 | Runtime frame behavior is driven by `JzWorld` systems.                                              |
| Service locator for shared services | Runtime services are exposed via `JzServiceContainer`.                                              |
| Multi-path rendering API            | Immediate-mode draw is the current default path; command list API is available as an optional path. |

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

- OpenGL: implemented and used
- Vulkan: planned

Notes on rendering command path:

- default runtime render flow uses immediate device calls
- `JzRHICommandList` exists as optional deferred recording utility

## Resource Layer

Key responsibilities:

- `JzAssetManager` lifecycle and registries
- type factories (`JzShaderAssetFactory`, `JzModelFactory`, ...)
- sync/async loading
- search-path based asset resolution
- ECS-facing integration via `JzAssetSystem`

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
3. `m_world->Update(delta)`
4. `OnRender(delta)`
5. clear input frame state
6. `m_graphicsContext->Present()`

`Present()` currently performs `device->Finish()` then `SwapBuffers()`.

## Editor Layer

The Editor uses runtime extension points rather than editor-specific runtime internals:

- `JzView` registers logical render targets.
- panels query `JzRenderOutput` by handle and display textures.
- editor feature drawing is appended as graph contributions
  with scope control (`MainScene` / `RegisteredTarget` / `All`).
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

### Immediate RHI Path (Current Default)

```cpp
device.BindPipeline(pipeline);
device.BindVertexArray(vertexArray);
device.DrawIndexed(drawParams);
```

### Command List (Available)

```cpp
auto cmd = device.CreateCommandList("Pass");
cmd->Begin();
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
- `src/Editor/Panels/src/JzView.cpp`
- `src/Editor/Application/src/JzREEditor.cpp`
