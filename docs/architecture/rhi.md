# JzRE RHI (Render Hardware Interface)

## Overview

JzRE RHI provides a cross-API rendering abstraction.

Current backend status:

- OpenGL: implemented and used in runtime/editor.
- Vulkan: implemented for runtime/editor, with automatic fallback to OpenGL on initialization failure.

## Current Runtime Rendering Path

The runtime render path is **command-list first** through `JzDevice`:

1. Runtime/editor code records commands to `JzRHICommandList`.
2. Backend executes the recorded list via `ExecuteCommandList(...)`.

There is no runtime immediate draw path in the `JzDevice` public API.

## Architecture

```mermaid
graph TB
    subgraph "Runtime/Editor"
        App[JzRERuntime / JzREEditor]
        Render[JzRenderSystem]
    end

    subgraph "RHI Abstraction"
        Context[JzGraphicsContext]
        Device[JzDevice]
        CmdList[JzRHICommandList (primary path)]
        Pipeline[JzRHIPipeline]
        Buffer[JzGPUBufferObject]
        Texture[JzGPUTextureObject]
        VAO[JzGPUVertexArrayObject]
        FBO[JzGPUFramebufferObject]
    end

    subgraph "OpenGL Backend"
        GLDevice[JzOpenGLDevice]
        GLPipeline[JzOpenGLPipeline]
        GLBuffer[JzOpenGLBuffer]
        GLTexture[JzOpenGLTexture]
        GLVAO[JzOpenGLVertexArray]
        GLFBO[JzOpenGLFramebuffer]
    end

    App --> Render
    App --> Context
    Context --> Device
    Render --> Device
    Device --> CmdList
    Device --> Pipeline
    Device --> Buffer
    Device --> Texture
    Device --> VAO
    Device --> FBO

    Device -.implements.-> GLDevice
    GLDevice --> GLPipeline
    GLDevice --> GLBuffer
    GLDevice --> GLTexture
    GLDevice --> GLVAO
    GLDevice --> GLFBO
```

## Core Components

### `JzGraphicsContext`

`JzGraphicsContext` owns:

- window backend pointer
- selected RHI type
- concrete `JzDevice`

Responsibilities:

- initialize device after making window context current
- frame boundaries (`BeginFrame`, `EndFrame`)
- presentation (`Present`)

Current `Present()` behavior:

1. OpenGL path: `device->Finish()` then `windowBackend->SwapBuffers()`
2. Vulkan path: device-side `Flush()` submit + `vkQueuePresentKHR`

### `JzDevice`

`JzDevice` is the central RHI abstraction:

- resource creation: pipeline, buffer, texture, shader, VAO, framebuffer
- command-list creation/execution (`CreateCommandList`, `ExecuteCommandList`, `ExecuteCommandLists`)
- frame lifecycle (`BeginFrame`, `EndFrame`, `Flush`, `Finish`)

### `JzRHICommandList` (Primary Path)

`JzRHICommandList` supports:

- `Begin` / `End`
- command recording (`BindFramebuffer`, `BindPipeline`, `SetViewport`, `Clear`, `DrawIndexed`, barriers, blit, ...)
- snapshot handoff to backend execution via `device.ExecuteCommandList(...)`

## RenderGraph and Barrier Integration

`JzRenderSystem` connects `JzRenderGraph` transitions to RHI barriers:

- graph transition callback builds `JzRHIResourceBarrier` list
- records barriers into pass command list via `commandList.ResourceBarrier(barriers)`
- graph execution runs via `JzRenderGraph::Execute(device)` and provides
  per-pass `JzRGPassContext` (framebuffer/viewport/resource bindings)

Backend behavior today:

- OpenGL implementation treats barriers as no-op (implicit transitions).
- Vulkan implementation applies texture layout transitions from recorded barriers.

## OpenGL Backend Notes

Implemented classes:

- `JzOpenGLDevice`
- `JzOpenGLBuffer`
- `JzOpenGLTexture`
- `JzOpenGLShader`
- `JzOpenGLPipeline`
- `JzOpenGLVertexArray`
- `JzOpenGLFramebuffer`

Backend characteristics:

- Version target: OpenGL 3.3+
- Multithreading support: `SupportsMultithreading() == false`
- `BeginFrame()` resets frame stats
- `EndFrame()` uses `glFlush()`
- `Finish()` uses `glFinish()`

## Runtime Integration Summary

In the runtime frame loop:

1. ECS systems update (`JzWorld::Update`).
2. `JzRenderSystem` records pass commands into `JzRHICommandList` through:
   built-in geometry contribution (`ResolveCameraFrameData -> BeginRenderTargetPass -> DrawVisibleEntities`)
   plus graph contribution passes (`JzRenderGraphContribution`).
   Editor integrations register these passes via `RegisterGraphContribution(...)`.
3. Host UI (`OnRender`) runs.
4. `JzGraphicsContext::Present()` performs finish + swap.

This is the behavior reflected by current code.

## Shutdown Ordering

Runtime shutdown uses a deterministic order to avoid backend resource lifetime issues:

1. `JzWorld::ShutdownSystems()` runs `OnShutdown` in reverse system registration order.
2. ECS system references are cleared (`m_systems.clear()`), then runtime-held system pointers are reset.
3. `JzGraphicsContext` and `JzDevice` are shut down after system-owned GPU resources are released.

For Vulkan this avoids late destruction of objects such as shader modules after `vkDestroyDevice`.
Additionally, Vulkan shader objects track a device lifetime flag and skip
`vkDestroyShaderModule` if the device teardown has already begun.

## Example: Command List Rendering

```cpp
auto cmd = device.CreateCommandList("MainPass");
cmd->Begin();
cmd->BindFramebuffer(framebuffer);
cmd->BindPipeline(pipeline);
cmd->BindVertexArray(vertexArray);
cmd->DrawIndexed(drawParams);
cmd->End();

device.ExecuteCommandList(cmd);
```

## Vulkan Runtime Notes

Current Vulkan backend includes:
- instance/surface/physical+logical device initialization
- swapchain + frame synchronization (frames-in-flight)
- per-frame `BeginFrame/EndFrame/Present` lifecycle
- swapchain render pass with color + depth attachments (depth-tested 3D rendering)
- Vulkan resource objects (`Buffer`, `Texture`, `Framebuffer`, `VertexArray`, `Shader`, `Pipeline`)
- pipeline setup:
  - descriptor set layouts from SPIR-V reflection
  - vertex input state from `JzPipelineDesc.vertexLayout` (generated from vertex shader `layout(location=...) in` declarations in shader asset/registry compile path)
  - SPIR-V reflected vertex input fallback when no explicit layout is provided
- descriptor-backed parameter binding for `SetUniform(...)` and `BindTexture(...)`:
  - uniform buffers are uploaded from pipeline parameter cache
  - combined image samplers are resolved from bound texture slots (with a fallback white texture)
- Editor ImGui Vulkan backend integration with texture bridge

Current compatibility note:
- `BlitFramebufferToScreen(...)` is treated as a Vulkan no-op in runtime direct-swapchain mode.

Default runtime/editor policy is platform auto-selection:
- prefer Vulkan (including macOS MoltenVK path)
- fallback to OpenGL with explicit logs when Vulkan is unavailable

## Source References

- `src/Runtime/Platform/src/RHI/JzGraphicsContext.cpp`
- `src/Runtime/Platform/include/JzRE/Runtime/Platform/RHI/JzDevice.h`
- `src/Runtime/Platform/src/OpenGL/JzOpenGLDevice.cpp`
- `src/Runtime/Platform/src/Command/JzRHICommandList.cpp`
- `src/Runtime/Function/src/ECS/JzRenderSystem.cpp`
