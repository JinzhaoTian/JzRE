# JzRE RHI (Render Hardware Interface)

## Overview

JzRE RHI provides a cross-API rendering abstraction.

Current backend status:

- OpenGL: implemented and used in runtime/editor.
- Vulkan: implemented for runtime/editor, with automatic fallback to OpenGL on initialization failure.

## Current Runtime Rendering Path

The current runtime render path is **immediate-mode device calls** through `JzDevice`:

- `BindFramebuffer`
- `BindPipeline`
- `SetViewport`
- `Clear`
- `BindVertexArray`
- `BindTexture`
- `Draw` / `DrawIndexed`

`JzRHICommandList` exists and can record commands, but the default runtime flow (`JzRenderSystem`) does not use command-list recording as its primary path.

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
        CmdList[JzRHICommandList (optional path)]
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
- draw API: immediate rendering commands
- optional command-list creation/execution
- resource barriers
- capability/statistics query

### `JzRHICommandList` (Optional Path)

`JzRHICommandList` supports:

- `Begin` / `End`
- command recording (`Clear`, `BindPipeline`, `DrawIndexed`, ...)
- execution through `device.ExecuteCommandList(...)`

This path is available, but current `JzRenderSystem` directly issues immediate commands.

## RenderGraph and Barrier Integration

`JzRenderSystem` connects `JzRenderGraph` transitions to RHI barriers:

- graph transition callback builds `JzRHIResourceBarrier` list
- calls `device.ResourceBarrier(barriers)` before pass execution
- graph execution runs via `JzRenderGraph::Execute(device)` and provides
  per-pass `JzRGPassContext` (framebuffer/viewport/resource bindings)

Backend behavior today:

- OpenGL implementation treats barriers as no-op (implicit transitions).

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
2. `JzRenderSystem` performs immediate RHI draw calls through:
   built-in geometry contribution (`ExecuteContribution` with
   `ResolveCameraFrameData -> BeginRenderTargetPass -> DrawVisibleEntities`,
   where the geometry pipeline is resolved from `shaders/standard` per frame
   and pass targets are provided by `JzRGPassContext`
   plus graph contribution passes (`JzRenderGraphContribution`).
   Editor integrations register these passes via `RegisterGraphContribution(...)`.
3. Host UI (`OnRender`) runs.
4. `JzGraphicsContext::Present()` performs finish + swap.

This is the behavior reflected by current code.

## Example: Immediate Rendering

```cpp
context.BeginFrame();

device.BindFramebuffer(framebuffer);
device.BindPipeline(pipeline);
device.BindVertexArray(vertexArray);
device.DrawIndexed(drawParams);

context.EndFrame();
context.Present();
```

## Example: Command List (Available)

```cpp
auto cmd = device.CreateCommandList("MainPass");
cmd->Begin();
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
- Vulkan resource objects (`Buffer`, `Texture`, `Framebuffer`, `VertexArray`, `Shader`, `Pipeline`)
- Editor ImGui Vulkan backend integration with texture bridge

Default runtime/editor policy is platform auto-selection:
- prefer Vulkan (including macOS MoltenVK path)
- fallback to OpenGL with explicit logs when Vulkan is unavailable

## Source References

- `src/Runtime/Platform/src/RHI/JzGraphicsContext.cpp`
- `src/Runtime/Platform/include/JzRE/Runtime/Platform/RHI/JzDevice.h`
- `src/Runtime/Platform/src/OpenGL/JzOpenGLDevice.cpp`
- `src/Runtime/Platform/src/Command/JzRHICommandList.cpp`
- `src/Runtime/Function/src/ECS/JzRenderSystem.cpp`
