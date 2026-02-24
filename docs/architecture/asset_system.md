# Asset System Architecture

This document describes the modern ECS-friendly asset management system for JzRE.

## Overview

The asset system provides:
- **Type-safe handles** with generation validation to prevent dangling references
- **Async loading** with priority-based queuing
- **LRU caching** with configurable memory budget
- **ECS integration** through dedicated components and systems

## Core Components

### JzAssetId

A unique identifier for assets consisting of:
- `index`: Slot index in the registry (U32)
- `generation`: Version counter incremented on reuse (U32)

```cpp
struct JzAssetId {
    U32 index      = 0xFFFFFFFF;
    U32 generation = 0;

    Bool IsValid() const;
    static constexpr JzAssetId Invalid();
};
```

The generation mechanism prevents use-after-free bugs: when a slot is reused, the generation increments, invalidating all old handles.

### JzAssetHandle<T>

Type-safe wrapper around JzAssetId:

```cpp
template<typename T>
class JzAssetHandle {
    JzAssetId m_id;
public:
    Bool IsValid() const;
    JzAssetId GetId() const;
};

// Type aliases
using JzMeshHandle     = JzAssetHandle<JzMesh>;
using JzTextureHandle  = JzAssetHandle<JzTexture>;
using JzMaterialHandle = JzAssetHandle<JzMaterial>;
```

### JzAssetRegistry<T>

Per-type storage for assets:

```cpp
template<typename T>
class JzAssetRegistry {
    std::vector<JzAssetSlot<T>> m_slots;
    std::queue<U32>             m_freeIndices;
    std::unordered_map<String, JzAssetHandle<T>> m_pathToHandle;

public:
    JzAssetHandle<T> Allocate(const String& path);
    void Free(JzAssetHandle<T> handle);
    T* Get(JzAssetHandle<T> handle);
    Bool IsValid(JzAssetHandle<T> handle) const;
};
```

Features:
- O(1) access by handle
- Automatic path-to-handle caching
- Thread-safe with shared_mutex
- Reference counting per slot

### JzAssetManager

Central coordinator for all asset operations:

```cpp
class JzAssetManager {
public:
    // Sync loading (blocking)
    template<typename T>
    JzAssetHandle<T> LoadSync(const String& path);

    // Async loading (non-blocking)
    template<typename T>
    JzAssetHandle<T> LoadAsync(const String& path,
                               JzAssetLoadCallback<T> callback,
                               I32 priority);

    // Access
    template<typename T>
    T* Get(JzAssetHandle<T> handle);

    // Per-frame update
    void Update();
};
```

Configuration:
```cpp
struct JzAssetManagerConfig {
    Size maxCacheMemoryMB     = 512;   // Memory budget
    Size asyncWorkerCount     = 2;     // Worker threads
    F32  lruEvictionThreshold = 0.8f;  // Eviction trigger
};
```

### JzLRUCacheManager

Tracks asset access for memory management:

```cpp
class JzLRUCacheManager {
public:
    void RecordAccess(JzAssetId id, Size memorySize);
    void Remove(JzAssetId id);

    std::vector<JzAssetId> GetEvictionCandidates(
        Size targetMemoryMB,
        const std::unordered_set<JzAssetId>& excludeIds);

    Bool IsOverBudget() const;
};
```

## Layer Architecture

The asset system follows strict layer separation:

```
┌─────────────────────────────────────────────────────────────┐
│                      Function Layer                         │
│              JzAssetSystem (ECS System Facade)              │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • Owns JzAssetManager (internal)                      │  │
│  │ • Absorbed JzAssetLoadingSystem logic                 │  │
│  │ • RegisterAsset<T>(): Encapsulates 4-step sequence    │  │
│  │ • SpawnModel(): Entity + Component + Asset setup      │  │
│  │ • AttachMesh/Material/Shader(): Component management  │  │
│  │ • DetachAllAssets(): Clean reference release          │  │
│  └───────────────────────────────────────────────────────┘  │
└────────────────────────────┬────────────────────────────────┘
                             │ GetAssetManager() [backward compat]
┌────────────────────────────▼────────────────────────────────┐
│                       Resource Layer                        │
│              JzAssetManager (Pure Asset Operations)         │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • No ECS knowledge                                    │  │
│  │ • JzAssetRegistry<T> per type                         │  │
│  │ • LoadSync/LoadAsync APIs                             │  │
│  │ • LRU cache management                                │  │
│  │ • Factory registration                                │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Design Principles

- **Resource Layer**: Pure C++ types, no ECS dependencies. Focus on asset operations.
- **Function Layer**: ECS system that bridges Resource layer with ECS. Handles component sync.
- **Backward Compatibility**: `JzAssetManager` still registered in `JzServiceContainer` for existing systems (`JzRenderSystem`, `JzAssetBrowser`).

## ECS Integration

### Asset Components

```cpp
// Reference a mesh asset
struct JzMeshAssetComponent {
    JzMeshHandle meshHandle;
    U32  indexCount = 0;      // Cached
    I32  materialIndex = -1;  // Cached
    Bool isReady = false;
};

// Reference a material asset
struct JzMaterialAssetComponent {
    JzMaterialHandle materialHandle;
    JzVec4 baseColor{1.0f};   // Cached
    Bool isReady = false;
};
```

### Asset Tags

```cpp
struct JzAssetLoadingTag {};     // Loading in progress
struct JzAssetReadyTag {};       // All assets ready
struct JzAssetLoadFailedTag {};  // Load failed

// Hot reload tags (added by JzAssetSystem when source files change)
struct JzShaderDirtyTag {};      // Cooked shader artifacts (.jzshader/.jzsblob) modified
struct JzTextureDirtyTag {};     // Texture file modified
struct JzMaterialDirtyTag {};    // Material definition modified
```

### JzAssetSystem

The main ECS system that coordinates loading state with ECS and provides high-level APIs. **Note:** `JzAssetSystem` has absorbed the functionality of the former `JzAssetLoadingSystem`.

```cpp
class JzAssetSystem : public JzSystem {
    JzSystemPhase GetPhase() const override {
        return JzSystemPhase::Logic;
    }

    void Update(JzWorld& world, F32 delta) override {
        // 1. Update asset manager (loading, LRU eviction)
        // 2. Process mesh/material/shader asset components
        // 3. Update component caches when loaded
        // 4. Manage state tags (Loading/Ready/Failed)
        // 5. Check for shader hot reload (JzShaderDirtyTag)
        // 6. Propagate dirty tags to dependent entities
    }

    // High-level APIs (replaces manual registry access)
    template<typename T>
    JzAssetHandle<T> RegisterAsset(const String& path,
                                   std::shared_ptr<T> asset);

    JzEntity SpawnModel(const String& path);
    void AttachMesh(JzEntity entity, JzMeshHandle handle);
    void AttachMaterial(JzEntity entity, JzMaterialHandle handle);
    void AttachShader(JzEntity entity, JzShaderHandle handle);
    void DetachAllAssets(JzEntity entity);

    // Hot reload
    void EnableHotReload(Bool enable);
    void CheckForModifiedAssets();
};
```

## Usage Examples

### Recommended: Using JzAssetSystem (Function Layer)

```cpp
// Access via runtime (recommended)
auto& assetSystem = runtime.GetAssetSystem();

// Initialize
JzAssetManagerConfig config;
assetSystem.Initialize(config);

// Register asset factories
assetSystem.RegisterFactory<JzMeshFactory>();
assetSystem.RegisterFactory<JzMaterialFactory>();
assetSystem.RegisterFactory<JzTextureFactory>();

// Add search paths
assetSystem.AddSearchPath("assets/models");
assetSystem.AddSearchPath("assets/textures");

// Sync load
auto meshHandle = assetSystem.LoadSync<JzMesh>("cube.obj");
JzMesh* mesh = assetSystem.Get(meshHandle);

// Register pre-loaded asset (encapsulates 4-step sequence)
Mesh = std::make_shared<JzMesh>(auto custom...);
auto handle = assetSystem.RegisterAsset("myMesh", customMesh);

// Spawn model entity (creates entity + components + asset refs)
JzEntity entity = assetSystem.SpawnModel("character.model");
assetSystem.AttachMaterial(entity, materialHandle);

// Async load
assetSystem.LoadAsync<JzTexture>("diffuse.png",
    [](JzTextureHandle handle, Bool success) {
        if (success) {
            // Texture ready
        }
    });
```

### ECS Usage

```cpp
// Create entity with asset components
auto entity = world.CreateEntity();

auto meshHandle = assetSystem.LoadAsync<JzMesh>("model.obj");
world.AddComponent<JzMeshAssetComponent>(entity, meshHandle);
world.AddComponent<JzTransformComponent>(entity);

// Register systems
world.RegisterSystem<JzAssetSystem>();
world.RegisterSystem<JzRenderSystem>();

// Render system can filter efficiently
auto view = registry.view<JzMeshAssetComponent,
                          JzTransformComponent,
                          JzAssetReadyTag>();
```

### Direct Access via Service Container (Backward Compatibility)

For existing code that accesses `JzAssetManager` directly:

```cpp
auto& assetManager = JzServiceContainer::Get<JzAssetManager>();
auto meshHandle = assetManager.LoadSync<JzMesh>("cube.obj");
```

Note: This works because `JzAssetSystem` internally registers `JzAssetManager` in the service container during initialization.

## Data Flow

```
Load Request
    │
    ▼
┌─────────────────────────────────┐
│         JzAssetSystem           │
│  (Entry point for all requests) │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│       JzAssetManager            │
│                                 │
│  LoadSync() / LoadAsync()       │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│       JzAssetRegistry<T>        │
│                                 │
│  Allocate slot                  │
│  Set LoadState                  │
└────────────┬────────────────────┘
             │
             ▼ (async)
┌─────────────────────────────────┐
│         ThreadPool              │
│                                 │
│  Worker loads via Factory       │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│       JzAssetRegistry<T>        │
│                                 │
│  Store asset                    │
│  Set Loaded                     │
└────────────┬────────────────────┘
             │
             ▼ (Update)
┌─────────────────────────────────┐
│        JzAssetSystem            │
│                                 │
│  UpdateAssetManager()           │
│   ProcessMesh/Mat/Shaders()     │
│  UpdateComponentCaches()        │
│  UpdateEntityAssetTags()        │
└─────────────────────────────────┘
             │
             ▼ (ECS Update)
┌─────────────────────────────────┐
│      JzWorld / Registry         │
│                                 │
│  Add/Update components          │
│  Add/Update tags                │
└─────────────────────────────────┘
```

## Memory Management

The LRU cache enforces memory budgets:

1. Every `Get()` call records access time
2. `Update()` checks memory usage ratio
3. When exceeding threshold, evict oldest unused assets
4. Assets with reference count > 0 are never evicted

## Thread Safety

- `JzAssetRegistry`: Uses `std::shared_mutex` for read-heavy workloads
- `JzAssetManager`: Separate mutexes for different subsystems
- `JzLRUCacheManager`: Simple mutex for entry tracking
- Callbacks invoked on main thread via result queue

## High-Level APIs

### RegisterAsset<T>

Replaces the 4-step manual sequence for registering pre-loaded assets:

```cpp
// Before (manual)
auto& registry = assetManager.GetRegistry<T>();
auto handle = registry.Allocate(path);
registry.Set(handle, std::move(asset));
registry.SetLoadState(handle, JzEAssetLoadState::Loaded);
assetManager.AddRef(handle);

// After (encapsulated)
auto handle = assetSystem.RegisterAsset(path, std::move(asset));
```

### SpawnModel

Creates an entity with all necessary components from a model file:

```cpp
// Create entity with mesh, material, and shader components
// All components are pre-configured with asset handles
// Entity automatically tagged with JzAssetReadyTag when loaded
JzEntity entity = assetSystem.SpawnModel("assets/character.model");
```

### Attach/Detach APIs

Simplify component management:

```cpp
// Attach assets to existing entity
assetSystem.AttachMesh(entity, meshHandle);
assetSystem.AttachMaterial(entity, materialHandle);

// Clean up all asset references on entity
assetSystem.DetachAllAssets(entity);
```

## Shader Asset Integration

The asset system includes built-in support for cooked shader packages through `JzShader` and `JzShaderFactory`.

### JzShader

A shader asset now represents a cooked runtime package:

```cpp
class JzShader : public JzResource {
public:
    // Load from cooked manifest (.jzshader)
    explicit JzShader(const String &shaderPath);

    // Get the default variant (keywordMask == 0)
    std::shared_ptr<JzRHIPipeline> GetMainVariant() const;

    // Get a variant by keyword mask
    std::shared_ptr<JzRHIPipeline> GetVariant(U64 keywordMask);

    // Build backend stage descriptors for one variant
    std::vector<JzShaderProgramDesc> GetBackendProgramDesc(
        JzERHIType rhiType, U64 keywordMask) const;
};
```

### Shader Loading

```cpp
// Load cooked shader via asset manager
auto shaderHandle = assetManager.LoadSync<JzShader>("shaders/standard.jzshader");

// Get the shader asset
JzShader* shader = assetManager.Get(shaderHandle);
if (shader && shader->IsCompiled()) {
    auto pipeline = shader->GetMainVariant();
    // Use pipeline for rendering
}
```

### Shader Variants

Shader variants are now selected via `keywordMask` (`U64`):

```cpp
// Example: enable bit 0 and bit 3
const U64 keywordMask = (1ull << 0) | (1ull << 3);
auto pipeline = shader->GetVariant(keywordMask);
```

### Cross-API Shader Contract

Offline cooking produces one package per shader with target payloads:

- OpenGL: GLSL text chunks
- Vulkan: SPIR-V binary chunks
- D3D12: DXIL binary chunks
- Metal: MSL text chunks

Runtime no longer compiles shader source in `JzShader`. It loads cooked payloads and creates pipelines directly.

### Hot Reload Support

`JzShader` hot reload now watches cooked files (`.jzshader` and `.jzsblob`). Hot reload is integrated into `JzAssetSystem`:

```cpp
// JzAssetSystem automatically monitors shader files
// When a shader file changes, entities with that shader get JzShaderDirtyTag

// Manual check (legacy approach):
if (shader->NeedsReload()) {
    shader->Reload();  // Recompiles all variants
}

// ECS-integrated approach (recommended):
auto view = world.View<JzShaderComponent, JzShaderDirtyTag>();
for (auto entity : view) {
    // Shader was hot-reloaded by JzAssetSystem
    // Handle re-binding uniforms, updating materials, etc.
    world.GetRegistry().remove<JzShaderDirtyTag>(entity);
}
```

### Hot Reload Integration in JzAssetSystem

`JzAssetSystem` integrates shader hot reload into the ECS update loop:

1. **File Watching**: Monitors cooked shader files for modifications
2. **Automatic Reload**: Reloads cooked manifests/blob chunks when files change
3. **Entity Tagging**: Adds `JzShaderDirtyTag` to all entities using the modified shader
4. **Cache Invalidation**: Drops cached variants and rebuilds on demand

This enables seamless iteration during development without manual reload calls.

### Factory Registration

The `JzShaderFactory` accepts cooked shader manifest paths:

```cpp
// 1. Explicit manifest path
assetManager.LoadSync<JzShader>("shaders/standard.jzshader");

// 2. Stem path (auto appends .jzshader)
assetManager.LoadSync<JzShader>("shaders/standard");
```

## Files

```
src/Runtime/Resource/include/JzRE/Runtime/Resource/
├── JzAssetId.h           # Asset ID with generation
├── JzAssetHandle.h       # Type-safe handle
├── JzAssetRegistry.h     # Per-type storage
├── JzAssetRegistry.inl   # Template implementation
├── JzAssetManager.h      # Central manager (Resource layer)
├── JzAssetManager.inl    # Template implementation
├── JzLRUCache.h          # Memory management
├── JzAssetHeaders.h      # Unified header (aggregator, renamed from JzAssetSystem.h)
├── JzShader.h       # Shader asset with variant support
└── JzShaderFactory.h # Factory for shader loading

src/Runtime/Resource/src/
├── JzAssetManager.cpp
├── JzLRUCache.cpp
└── JzShader.cpp

src/Runtime/Function/include/JzRE/Runtime/Function/ECS/
├── JzAssetComponents.h       # ECS components (mesh, material, shader)
└── JzAssetSystem.h           # Main ECS system (Function layer facade)
└── JzAssetSystem.inl         # Template implementations

src/Runtime/Function/src/ECS/
└── JzAssetSystem.cpp         # Full implementation (absorbed JzAssetLoadingSystem)
```
