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
struct JzAssetLoadingTag {};    // Loading in progress
struct JzAssetReadyTag {};      // All assets ready
struct JzAssetLoadFailedTag {}; // Load failed
```

### JzAssetLoadingSystem

Coordinates loading state with ECS:

```cpp
class JzAssetLoadingSystem : public JzEnttSystem {
    JzSystemPhase GetPhase() const override {
        return JzSystemPhase::Logic;
    }

    void Update(JzEnttWorld& world, F32 delta) override {
        // 1. Check asset load states
        // 2. Update component caches when loaded
        // 3. Manage state tags
    }
};
```

## Usage Examples

### Basic Loading

```cpp
// Initialize
JzAssetManager assetManager;
assetManager.Initialize();
JzServiceContainer::Provide<JzAssetManager>(assetManager);

// Sync load
auto meshHandle = assetManager.LoadSync<JzMesh>("cube.obj");
JzMesh* mesh = assetManager.Get(meshHandle);

// Async load
assetManager.LoadAsync<JzTexture>("diffuse.png",
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

auto meshHandle = assetManager.LoadAsync<JzMesh>("model.obj");
world.AddComponent<JzMeshAssetComponent>(entity, meshHandle);
world.AddComponent<JzTransformComponent>(entity);

// Register systems
world.RegisterSystem<JzAssetLoadingSystem>();
world.RegisterSystem<JzEnttRenderSystem>();

// Render system can filter efficiently
auto view = registry.view<JzMeshAssetComponent,
                          JzTransformComponent,
                          JzAssetReadyTag>();
```

## Data Flow

```
Load Request
    │
    ▼
┌─────────────────┐
│  AssetManager   │──── Sync: Load immediately
│                 │
│  LoadAsync()    │──── Async: Queue for workers
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  AssetRegistry  │
│                 │
│  Allocate slot  │
│  Set Loading    │
└────────┬────────┘
         │
         ▼ (async)
┌─────────────────┐
│  ThreadPool     │
│                 │
│  Worker loads   │
│  via Factory    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  AssetRegistry  │
│                 │
│  Store asset    │
│  Set Loaded     │
└────────┬────────┘
         │
         ▼ (Update)
┌─────────────────┐
│ AssetLoadingSystem │
│                    │
│ Update component   │
│ cache & tags       │
└────────────────────┘
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

## Comparison with JzResourceManager

| Feature | JzResourceManager | JzAssetManager |
|---------|-------------------|----------------|
| Handle type | `std::shared_ptr<T>` | `JzAssetHandle<T>` |
| Generation check | No | Yes |
| Async loading | No (TODO) | Yes |
| Memory budget | No | Yes (LRU) |
| ECS integration | Manual | Automatic |
| Thread safety | Mutex per cache | Fine-grained |

## Migration Path

1. **Phase 1**: Use new system for new code
2. **Phase 2**: Add adapter for existing components
3. **Phase 3**: Gradually migrate existing code
4. **Phase 4**: Deprecate old components

Both systems can coexist during migration.

## Files

```
src/Runtime/Resource/include/JzRE/Runtime/Resource/
├── JzAssetId.h           # Asset ID with generation
├── JzAssetHandle.h       # Type-safe handle
├── JzAssetRegistry.h     # Per-type storage
├── JzAssetRegistry.inl   # Template implementation
├── JzAssetManager.h      # Central manager
├── JzAssetManager.inl    # Template implementation
├── JzLRUCache.h          # Memory management
└── JzAssetSystem.h       # Unified header

src/Runtime/Resource/src/
├── JzAssetManager.cpp
└── JzLRUCache.cpp

src/Runtime/Function/include/JzRE/Runtime/Function/ECS/
├── JzAssetComponents.h       # ECS components
└── JzAssetLoadingSystem.h    # Loading coordinator

src/Runtime/Function/src/ECS/
└── JzAssetLoadingSystem.cpp
```
