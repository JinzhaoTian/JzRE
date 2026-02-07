# JzRE Rendering Pipeline

## Overview

This document describes the ECS-based rendering pipeline in JzRE. The rendering is handled by systems registered in `JzWorld` and executed in registration order each frame. Editor views (SceneView/AssetView/GameView) register render targets with `JzRenderSystem` lazily (after construction, once the render system is available) and display the generated textures in ImGui.

---

## Architecture

```
JzRERuntime
  └── JzWorld
        ├── JzWindowSystem - Window polling, input state sync
        ├── JzInputSystem  - Input processing, event emission
        ├── JzEventSystem  - Event dispatch
        ├── JzAssetSystem  - Asset loading, hot reload
        ├── JzCameraSystem - Camera matrices, orbit control
        ├── JzLightSystem  - Light data collection
        └── JzRenderSystem - RenderGraph, targets, draw calls
```

### System Phases

Systems execute in 8 phases grouped into 3 categories:

| Group         | Phase      | Purpose                                    | Example Systems                           |
| ------------- | ---------- | ------------------------------------------ | ----------------------------------------- |
| **Logic**     | Input      | Input processing, event handling           | JzWindowSystem, JzInputSystem             |
|               | Physics    | Physics simulation, collision detection    | PhysicsSystem (user-defined)              |
|               | Animation  | Skeletal animation, blend trees            | AnimationSystem (user-defined)            |
|               | Logic      | General game logic, AI, scripts            | JzAssetSystem, JzMoveSystem, user systems |
| **PreRender** | PreRender  | Camera matrices, light collection          | JzCameraSystem, JzLightSystem             |
|               | Culling    | Frustum culling, occlusion, LOD selection  | CullingSystem (user-defined)              |
| **Render**    | RenderPrep | Batch building, instance data preparation  | BatchBuildingSystem (user-defined)        |
|               | Render     | Actual GPU draw calls                      | JzRenderSystem                            |

### System Responsibilities

| System                 | Phase     | Responsibilities                                                 |
| ---------------------- | --------- | ---------------------------------------------------------------- |
| **JzCameraSystem** | PreRender | Process orbit controller input, compute view/projection matrices |
| **JzLightSystem**  | PreRender | Collect light entities, provide primary light direction/color    |
| **JzRenderSystem** | Render    | Manage framebuffer/textures, build RenderGraph, render entities and view targets |

---

## Frame Execution Flow

The main loop in `JzRERuntime::Run()` updates `JzWorld` once per frame. `JzWorld::Update()` runs registered systems in the order shown above.

```
[Phase 1: Frame Start - Input and Window Events]
   m_window->PollEvents()
   |
   v
[Phase 2: Async Processing - Start Background Tasks]
   _SignalWorkerFrame(frameData)
   |
   v
1. `OnUpdate(deltaTime)` runs editor/game logic (e.g., panel updates).
2. `m_world->Update(deltaTime)` runs systems in registration order:
   - `JzWindowSystem`/`JzInputSystem` update input and window state.
   - `JzAssetSystem` updates asset state.
   - `JzCameraSystem` computes view/projection matrices.
   - `JzLightSystem` collects light data.
   - `JzRenderSystem` builds/executes the RenderGraph.
3. `OnRender(deltaTime)` draws ImGui UI (editor panels read from render targets).
```

---

## Phase Separation Benefits

1. **Parallel Execution**: Logic systems can run in parallel with GPU work from previous frame
2. **Clear Synchronization**: Explicit sync point ensures background tasks complete before rendering
3. **Data Isolation**: PreRender phase prepares render data after logic updates are complete
4. **Extensibility**: Easy to add new systems by specifying their phase

---

## Key Components

### Camera Components

```cpp
// Full camera state
struct JzCameraComponent {
    JzVec3 position;
    JzVec4 rotation;        // pitch, yaw, roll, unused
    F32    fov, nearPlane, farPlane, aspect;
    JzVec3 clearColor;
    Bool   isMainCamera;
    JzMat4 viewMatrix;       // Computed by CameraSystem
    JzMat4 projectionMatrix; // Computed by CameraSystem
};

// Orbit controller for camera
struct JzOrbitControllerComponent {
    JzVec3 target;
    F32    yaw, pitch, distance;
    F32    orbitSensitivity, panSensitivity, zoomSensitivity;
    F32    minDistance, maxDistance;
    // Mouse tracking state...
};
```

### Light Components

```cpp
struct JzDirectionalLightComponent {
    JzVec3 direction;
    JzVec3 color;
    F32    intensity;
};

struct JzPointLightComponent {
    JzVec3 color;
    F32    intensity, range;
    F32    constant, linear, quadratic;  // Attenuation
};

struct JzSpotLightComponent {
    JzVec3 direction, color;
    F32    intensity, range;
    F32    innerCutoff, outerCutoff;  // Cone angles
};
```

### Renderable Entity

An entity is rendered if it has these components:

| Component              | Purpose                            |
| ---------------------- | ---------------------------------- |
| `JzTransformComponent` | Position, rotation, scale          |
| `JzMeshComponent`      | Reference to `JzMesh` resource     |
| `JzMaterialComponent`  | Reference to `JzMaterial` resource |

---

## Code Example

### Rendering Flow in JzRenderSystem

```cpp
void JzRenderSystem::Update(JzWorld &world, F32 delta) {
    // Create/recreate framebuffer if size changed
    if (m_frameSizeChanged) {
        CreateFramebuffer();
    }

    // Setup viewport and clear
    SetupViewportAndClear();

    // Get camera data from CameraSystem
    JzMat4 viewMatrix = m_cameraSystem->GetViewMatrix();
    JzMat4 projMatrix = m_cameraSystem->GetProjectionMatrix();
    JzVec3 cameraPos = m_cameraSystem->GetCameraPosition();

    // Get light data from LightSystem
    JzVec3 lightDir = m_lightSystem->GetPrimaryLightDirection();
    JzVec3 lightColor = m_lightSystem->GetPrimaryLightColor();

    // Set common uniforms
    m_pipeline->SetUniform("view", viewMatrix);
    m_pipeline->SetUniform("projection", projMatrix);
    m_pipeline->SetUniform("uCameraPos", cameraPos);
    m_pipeline->SetUniform("uLightDir", lightDir);
    m_pipeline->SetUniform("uLightColor", lightColor);

    // Render all entities with Transform + Mesh + Material
    auto view = world.View<JzTransformComponent, JzMeshComponent, JzMaterialComponent>();

    for (auto entity : view) {
        auto &transform = world.GetComponent<JzTransformComponent>(entity);
        auto &meshComp = world.GetComponent<JzMeshComponent>(entity);
        auto &matComp = world.GetComponent<JzMaterialComponent>(entity);

        auto mesh = std::static_pointer_cast<JzMesh>(meshComp.mesh);
        auto material = std::static_pointer_cast<JzMaterial>(matComp.material);

        if (!mesh || mesh->GetState() != JzEResourceState::Loaded) continue;

        // Set model matrix
        JzMat4 modelMatrix = ComputeModelMatrix(transform);
        m_pipeline->SetUniform("model", modelMatrix);

        // Set material uniforms
        if (material && material->GetState() == JzEResourceState::Loaded) {
            const auto &props = material->GetProperties();
            m_pipeline->SetUniform("uAmbientColor", props.ambientColor);
            m_pipeline->SetUniform("uDiffuseColor", props.diffuseColor);
            m_pipeline->SetUniform("uSpecularColor", props.specularColor);
            m_pipeline->SetUniform("uShininess", props.shininess);
        }

        // Draw
        m_device->BindVertexArray(mesh->GetVertexArray());
        m_device->DrawIndexed(drawParams);
    }

    // Unbind framebuffer
    m_device->BindFramebuffer(nullptr);
}
```

---

## Model Spawning

Use `JzModelSpawner` to create entities from model files:

```cpp
// Load model
auto model = std::make_shared<JzModel>("models/scene.obj");
model->Load();

// Spawn entities (one per mesh)
auto entities = JzModelSpawner::SpawnModel(world, model);

// Each entity has:
// - JzTransformComponent
// - JzMeshComponent
// - JzMaterialComponent
// - JzRenderableTag
```

---

## Editor Integration

Editor views register a render view and retrieve the output via `JzRenderSystem`:

```cpp
// In JzView: after registration
auto *output = renderSystem.GetRenderOutput(GetOutputName());
if (output && output->IsValid()) {
    ImGui::Image(output->GetTextureID(), ImVec2(viewportWidth, viewportHeight));
}
```

The Editor overrides `ShouldBlitToScreen()` to return `false`, preventing automatic screen blit.

---

## Sequence Diagram

```mermaid
sequenceDiagram
    participant App as JzRERuntime
    participant Worker as WorkerThread
    participant World as JzWorld
    participant Camera as JzCameraSystem
    participant Light as JzLightSystem
    participant Render as JzRenderSystem
    participant GPU as GPU/RHI

    Note over App: Phase 1: Frame Start
    App->>App: PollEvents()

    Note over App,Worker: Phase 2: Async Processing
    App->>Worker: SignalWorkerFrame()

    Note over App,World: Phase 3: Logic Update (parallel with Worker)
    App->>World: UpdateLogic(deltaTime)
    World->>World: Logic systems (movement, physics, AI)

    App->>App: OnUpdate(deltaTime)

    Note over App,Worker: Phase 4: Sync Point
    App->>Worker: WaitForWorkerComplete()

    Note over App,Light: Phase 5: Pre-Render Update
    App->>World: UpdatePreRender(deltaTime)
    World->>Camera: Update()
    Camera->>Camera: Process orbit input
    Camera->>Camera: Compute matrices
    World->>Light: Update()
    Light->>Light: Collect light entities

    Note over App,Render: Phase 6: Render Update
    App->>World: UpdateRender(deltaTime)
    World->>Render: Update()
    Render->>Render: Setup framebuffer
    Render->>Camera: GetViewMatrix()
    Render->>Light: GetPrimaryLightDirection()
    Render->>GPU: BindFramebuffer()
    Render->>GPU: Clear()

    loop For each renderable entity
        Render->>GPU: SetUniforms()
        Render->>GPU: BindVertexArray()
        Render->>GPU: DrawIndexed()
    end

    Render->>GPU: UnbindFramebuffer()

    Note over App,GPU: Phase 7: Execute Rendering
    App->>Render: BeginFrame()
    App->>Render: EndFrame()

    alt Standalone Runtime
        App->>Render: BlitToScreen()
        Render->>GPU: BlitFramebuffer()
    end

    App->>App: OnRender(deltaTime)

    Note over App: Phase 8: Frame End
    App->>App: SwapBuffers()
    App->>App: ClearEvents()
```

---

## Key Type Relationships

| Component               | Resource     | RHI Objects                                   |
| ----------------------- | ------------ | --------------------------------------------- |
| `JzMeshComponent`       | `JzMesh`     | `JzGPUVertexArrayObject`, `JzGPUBufferObject` |
| `JzMaterialComponent`   | `JzMaterial` | `JzRHIPipeline`, `JzGPUTextureObject`         |
| `JzCameraComponent` | -            | Uniform data                                  |
| `Jz*LightComponent` | -            | Uniform data                                  |

---

## Default Shaders

The `JzRenderSystem` creates a default pipeline with Blinn-Phong shading:

**Vertex Shader:**

- MVP transformation
- Pass normal and world position to fragment

**Fragment Shader:**

- Ambient + Diffuse + Specular lighting
- Material colors: `uAmbientColor`, `uDiffuseColor`, `uSpecularColor`, `uShininess`
- Light: `uLightDir`, `uLightColor`
- Camera: `uCameraPos`
