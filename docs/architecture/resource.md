# JzRE Resource Layer Design

## Overview

The Resource Layer is a core component of the JzRE engine, responsible for managing the lifecycle of all game assets (textures, models, materials, shaders, etc.). The design is based on three core concepts: `JzResource`, `JzResourceFactory`, and `JzResourceManager`.

---

## Design Goals

- **Automation**: Automatic resource loading, unloading, and caching
- **Extensibility**: Easy support for new resource types without modifying core code
- **Efficiency**: Reference-counted memory management
- **Decoupling**: Resource management logic isolated from other engine parts

---

## Core Components

### JzResource (Resource Base Class)

Abstract base class for all resource types.

```cpp
enum class JzEResourceState {
    Unloaded,  // Not loaded
    Loading,   // Loading (reserved)
    Loaded,    // Loaded
    Error      // Error
};

class JzResource {
public:
    virtual ~JzResource() = default;

    JzEResourceState GetState() const { return m_state; }
    const String& GetName() const { return m_name; }

    virtual Bool Load() = 0;    // Load resource
    virtual void Unload() = 0;  // Unload resource

protected:
    JzEResourceState m_state = JzEResourceState::Unloaded;
    String m_name;
};
```

### JzResourceFactory (Resource Factory)

Abstract factory interface for creating concrete resource type instances.

```cpp
class JzResourceFactory {
public:
    virtual ~JzResourceFactory() = default;
    virtual JzResource* Create(const String& name) = 0;
};
```

### JzResourceManager (Resource Manager)

Central coordinator for resource management, the sole entry point for obtaining resources.

```cpp
class JzResourceManager {
public:
    // Register resource factory
    template <typename T>
    void RegisterFactory(std::unique_ptr<JzResourceFactory> factory);

    // Get resource (auto-loads)
    template <typename T>
    std::shared_ptr<T> GetResource(const String& name);

    // Clean up unused resources
    void UnloadUnusedResources();

    // Search path management
    void AddSearchPath(const String& path);
    String FindFullPath(const String& relativePath);

private:
    std::unordered_map<std::type_index, std::unique_ptr<JzResourceFactory>> m_factories;
    std::unordered_map<String, std::weak_ptr<JzResource>> m_resourceCache;
    std::vector<String> m_searchPaths;
    std::mutex m_cacheMutex;
};
```

---

## Resource Types

### Implemented Resource Types

| Resource Type | Class Name | Factory Class | Purpose |
|---------------|------------|---------------|---------|
| Texture | `JzTexture` | `JzTextureFactory` | Image resources |
| Mesh | `JzMesh` | `JzMeshFactory` | Geometry data |
| Material | `JzMaterial` | `JzMaterialFactory` | Render materials |
| Shader | `JzShader` | `JzShaderFactory` | GPU programs |
| Model | `JzModel` | `JzModelFactory` | Complete 3D models |
| Font | `JzFont` | `JzFontFactory` | Text rendering |

### Resource Class Relationships

```mermaid
classDiagram
    class JzResource {
        <<abstract>>
        +GetState() JzEResourceState
        +GetName() String
        +Load()* Bool
        +Unload()* void
        #m_state
        #m_name
    }

    class JzTexture {
        +GetRHITexture() JzGPUTextureObject*
        -m_rhiTexture
    }

    class JzMesh {
        +GetVertexArray() JzGPUVertexArrayObject*
        +GetVertexBuffer() JzGPUBufferObject*
        +GetIndexBuffer() JzGPUBufferObject*
        -m_vertices
        -m_indices
    }

    class JzMaterial {
        +GetPipeline() JzRHIPipeline*
        +GetTextures() vector
        -m_pipeline
        -m_textures
    }

    class JzShader {
        +GetProgram() JzGPUShaderProgramObject*
        -m_program
    }

    class JzModel {
        +GetMeshes() vector
        +GetMaterials() vector
        -m_meshes
        -m_materials
    }

    JzResource <|-- JzTexture
    JzResource <|-- JzMesh
    JzResource <|-- JzMaterial
    JzResource <|-- JzShader
    JzResource <|-- JzModel

    JzModel *-- JzMesh
    JzModel *-- JzMaterial
    JzMaterial *-- JzTexture
    JzMaterial *-- JzShader
```

---

## Workflow

### 1. Resource Registration (Initialization Phase)

```cpp
// Register all factories during engine initialization
auto& resourceManager = JzServiceContainer::Get<JzResourceManager>();

resourceManager.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
resourceManager.RegisterFactory<JzMesh>(std::make_unique<JzMeshFactory>());
resourceManager.RegisterFactory<JzMaterial>(std::make_unique<JzMaterialFactory>());
resourceManager.RegisterFactory<JzShader>(std::make_unique<JzShaderFactory>());
resourceManager.RegisterFactory<JzModel>(std::make_unique<JzModelFactory>());

// Add resource search paths
resourceManager.AddSearchPath("./resources");
resourceManager.AddSearchPath("./assets");
```

### 2. Resource Loading

```cpp
// Request texture resource (manager auto-handles caching and loading)
auto texture = resourceManager.GetResource<JzTexture>("textures/player.png");

// Check resource state
if (texture && texture->GetState() == JzEResourceState::Loaded) {
    // Use texture
    auto rhiTexture = texture->GetRHITexture();
}
```

### 3. Internal Loading Process

```mermaid
flowchart TD
    A[GetResource Call] --> B{Cache Lookup}
    B -->|Hit and Valid| C[Return shared_ptr]
    B -->|Miss or Expired| D{Find Factory}
    D -->|Not Found| E[Return nullptr]
    D -->|Found| F[Factory Creates Resource]
    F --> G[Call Load]
    G --> H[Add to Cache]
    H --> C
```

### 4. Resource Unloading

Resource unloading is handled automatically through reference counting:

```cpp
// When the last shared_ptr is destroyed, resource is automatically unloaded
{
    auto texture = resourceManager.GetResource<JzTexture>("textures/enemy.png");
    // Use texture...
} // texture goes out of scope, if no other references, resource is destroyed

// Periodically clean up expired cache entries
resourceManager.UnloadUnusedResources();
```

---

## Implementation Details

### GetResource Template Implementation

```cpp
template <typename T>
std::shared_ptr<T> JzResourceManager::GetResource(const String& name) {
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    // 1. Cache lookup
    auto it = m_resourceCache.find(name);
    if (it != m_resourceCache.end()) {
        if (auto sharedRes = it->second.lock()) {
            return std::static_pointer_cast<T>(sharedRes);
        }
    }

    // 2. Cache miss: create new resource
    auto factory_it = m_factories.find(std::type_index(typeid(T)));
    if (factory_it == m_factories.end()) {
        // Error: no factory registered for this type
        return nullptr;
    }

    JzResource* newRawRes = factory_it->second->Create(name);
    std::shared_ptr<T> newRes = std::shared_ptr<T>(static_cast<T*>(newRawRes));

    // 3. Load and cache
    newRes->Load();
    m_resourceCache[name] = newRes;

    return newRes;
}
```

### Factory Implementation Examples

```cpp
// JzTextureFactory.h
class JzTextureFactory : public JzResourceFactory {
public:
    JzResource* Create(const String& name) override {
        return new JzTexture(name);
    }
};

// JzShaderFactory.h (with parameters)
class JzShaderFactory : public JzResourceFactory {
public:
    JzResource* Create(const String& name) override {
        // Parse shader paths
        String vertPath = name + ".vert";
        String fragPath = name + ".frag";
        return new JzShader(vertPath, fragPath);
    }
};
```

---

## Caching Strategy

### weak_ptr Automatic Cleanup

Key advantages of using `std::weak_ptr` as cache values:

1. **Automatic Memory Reclamation**: Resources are automatically freed when all `shared_ptr`s are destroyed
2. **No Dangling References**: Cache doesn't prevent resource reclamation
3. **On-Demand Reload**: Expired resources can be transparently reloaded

```cpp
void JzResourceManager::UnloadUnusedResources() {
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    for (auto it = m_resourceCache.begin(); it != m_resourceCache.end(); ) {
        if (it->second.expired()) {
            it = m_resourceCache.erase(it);
        } else {
            ++it;
        }
    }
}
```

---

## ECS Integration

Resources integrate with the ECS system through components:

```cpp
// Components hold resource references
struct JzMeshComponent {
    std::shared_ptr<JzResource> mesh;
};

struct JzMaterialComponent {
    std::shared_ptr<JzResource> material;
};

// Render system uses resources
void JzRenderSystem::Update(JzEntityManager& manager, F32 delta) {
    for (auto entity : manager.View<JzMeshComponent, JzMaterialComponent, JzTransformComponent>()) {
        auto& meshComp = manager.GetComponent<JzMeshComponent>(entity);
        auto& matComp = manager.GetComponent<JzMaterialComponent>(entity);

        auto mesh = std::static_pointer_cast<JzMesh>(meshComp.mesh);
        auto material = std::static_pointer_cast<JzMaterial>(matComp.material);

        if (mesh && material && mesh->GetState() == JzEResourceState::Loaded) {
            m_device->BindPipeline(material->GetPipeline());
            m_device->BindVertexArray(mesh->GetVertexArray());
            // ... draw
        }
    }
}
```

---

## Usage Examples

### Loading a Model

```cpp
auto& resMgr = JzServiceContainer::Get<JzResourceManager>();

// Load complete model (with meshes and materials)
auto model = resMgr.GetResource<JzModel>("models/character.fbx");

if (model && model->GetState() == JzEResourceState::Loaded) {
    for (auto& mesh : model->GetMeshes()) {
        // Process each mesh
    }
}
```

### Using Textures in UI

```cpp
// Load icon texture
auto iconTexture = resourceManager.GetResource<JzTexture>("icons/button.png");

if (iconTexture) {
    auto myButton = std::make_unique<JzImageButton>(
        iconTexture->GetRHITexture(),
        JzVec2{24.0f, 24.0f}
    );
    myButton->ClickedEvent.AddListener([]() {
        // Handle click
    });
}
```

---

## Future Plans

> [!NOTE]
> The following features are planned for future versions and are not currently implemented.

### Async Loading (Low Priority)

```cpp
// Future API design reference
template<typename T>
std::future<std::shared_ptr<T>> GetResourceAsync(const String& name);
```

### Resource Dependencies

Automatic loading of dependent resources (e.g., materials auto-loading referenced textures).

### Hot Reload

Automatic resource reloading when files change in editor mode.

### Streaming

Chunked loading for large resources (large textures, terrain).
