# JzRE Resource Layer Design

## 1. Overview

The resource layer is a critical component of JzRE, responsible for managing the lifecycle of all game assets (e.g., textures, models, materials, shaders). This design is based on three core concepts: `JzResource`, `JzResourceFactory`, and `JzResourceManager`.

The primary goals of this architecture are:

-   **Automation**: Automate resource loading, unloading, and caching.
-   **Extensibility**: Easily support new resource types without modifying core manager code.
-   **Efficiency**: Use reference counting to manage memory and support asynchronous loading to avoid blocking the main thread.
-   **Decoupling**: Isolate resource management logic from the rest of the engine.

## 2. Core Components

### 2.1. `JzResource`

This is the abstract base class for all resource types.

-   **`State`**: Tracks the current status of the resource (`Unloaded`, `Loading`, `Loaded`, `Error`).
-   **`m_name`**: A unique string identifier for the resource, typically its relative path (e.g., "textures/player.png").
-   **`Load()` / `Unload()`**: Pure virtual functions that concrete resource classes must implement to handle their specific loading (e.g., from disk to GPU) and unloading logic.

### 2.2. `JzResourceFactory`

This is an abstract factory interface used to create instances of specific resource types.

-   For each concrete resource class, like `JzTexture`, a corresponding factory, `JzTextureFactory`, will be implemented.
-   The manager uses these factories to instantiate resources without needing to know their concrete types.

### 2.3. `JzResourceManager`

This is the central singleton or service that orchestrates all resource management. It is the only class that client code should interact with to obtain resources.

-   **Cache (`m_resourceCache`)**: An `std::unordered_map<std::string, std::weak_ptr<JzResource>>`. Using `std::weak_ptr` is key to automatic memory management. It allows the manager to track resources without preventing them from being deallocated when they are no longer in use (i.e., all `std::shared_ptr` instances are gone).
-   **Factories (`m_factories`)**: A map (`std::unordered_map<std::type_index, std::unique_ptr<JzResourceFactory>>`) to store and look up the appropriate factory for a given resource type.
-   **Search Paths (`m_searchPaths`)**: A list of directories where the manager should look for resource files.

## 3. Workflow

### 3.1. Resource Registration (Initialization Phase)

Before the resource manager can be used, all resource types must be registered.

```cpp
// In the engine's initialization phase
auto& resourceManager = JzServiceContainer::Get<JzResourceManager>();

// Register a factory for JzTexture
resourceManager.RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());

// Register a factory for JzModel
resourceManager.RegisterFactory<JzModel>(std::make_unique<JzModelFactory>());

// Add paths to search for resource files
resourceManager.AddSearchPath("./resources");
resourceManager.AddSearchPath("./assets");
```

### 3.2. Resource Loading

The `GetResource<T>(name)` method is the primary way to request a resource.

```cpp
// Request a texture. The manager handles everything else.
std::shared_ptr<JzTexture> myTexture = resourceManager.GetResource<JzTexture>("textures/player.png");
```

The internal process is as follows:

1.  **Cache Lookup**: The manager first searches `m_resourceCache` using the resource `name`.
2.  **Cache Hit**:
    -   If a `weak_ptr` is found and it's not expired (`!weak_ptr.expired()`), it means the resource is still active.
    -   The `weak_ptr` is promoted to a `shared_ptr` and returned immediately.
3.  **Cache Miss**:
    -   If no entry is found or the `weak_ptr` has expired, a new resource must be created.
    -   The manager uses `typeid(T)` to find the corresponding `JzResourceFactory` in its `m_factories` map.
    -   The factory's `Create(name)` method is called to instantiate a new resource object, which is wrapped in a `std::shared_ptr`.
    -   The new `shared_ptr` is used to create a `weak_ptr` which is then stored in `m_resourceCache`.
    -   The resource's `Load()` method is called. This can be synchronous or asynchronous.
    -   The `shared_ptr` to the new resource is returned.

### 3.3. Asynchronous Loading

To prevent stalling the render loop, loading can be done in the background.

1.  `GetResource<T>(name, true)` is called with the async flag.
2.  The manager creates the resource instance as usual, but instead of calling `Load()` directly, it pushes a load task onto a thread-safe queue (`m_asyncLoadQueue`). The resource's state is set to `State::Loading`.
3.  A pool of worker threads consumes tasks from this queue, executing the `Load()` method.
4.  The manager's `Update()` method, called every frame, checks for completed tasks. When a task is done, it updates the resource's state to `State::Loaded` or `State::Error`.
5.  Client code can check `resource->GetState()` to see if the resource is ready for use.

### 3.4. Resource Unloading

Unloading is handled automatically by reference counting.

1.  When the last `shared_ptr` pointing to a resource goes out of scope, the resource's destructor is called.
2.  The destructor should call the `Unload()` method to release any acquired resources (e.g., GPU memory).
3.  The `JzResourceManager::UnloadUnusedResources()` method can be called periodically (e.g., when changing scenes) to clean up the `m_resourceCache` by removing entries whose `weak_ptr`s have expired.

## 4. Proposed Implementation Details

### 4.1. `JzResourceManager.h` (Enhanced)

```cpp
#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Resource/JzResource.h"
#include "JzResourceFactory.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <typeindex>
#include <mutex>

namespace JzRE {

class JzResourceManager {
public:
    // Registers a factory for a given resource type T
    template <typename T>
    void RegisterFactory(std::unique_ptr<JzResourceFactory> factory);

    // Gets a resource. Creates and loads it if not available.
    template <typename T>
    std::shared_ptr<T> GetResource(const String& name);

    // Called every frame to process async queue, etc.
    void Update();

    // Cleans up expired weak_ptrs from the cache.
    void UnloadUnusedResources();

    void AddSearchPath(const std::string& path);
    String FindFullPath(const String& relativePath);

private:
    std::unordered_map<String, std::weak_ptr<JzResource>> m_resourceCache;
    std::vector<String> m_searchPaths;
    std::unordered_map<std::type_index, std::unique_ptr<JzResourceFactory>> m_factories;

    std::mutex m_cacheMutex;

    // TODO: Add async loading members (queue, threads, etc.)
};

// --- Template Implementations ---

template <typename T>
void JzResourceManager::RegisterFactory(std::unique_ptr<JzResourceFactory> factory) {
    m_factories[std::type_index(typeid(T))] = std::move(factory);
}

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

    // 2. Cache miss or expired: Create new resource
    auto factory_it = m_factories.find(std::type_index(typeid(T)));
    if (factory_it == m_factories.end()) {
        // Log error: No factory registered for this type
        return nullptr;
    }

    JzResource* newRawRes = factory_it->second->Create(name);
    std::shared_ptr<T> newRes = std::shared_ptr<T>(static_cast<T*>(newRawRes));

    // 3. Load and cache
    newRes->Load(); // For now, synchronous load
    m_resourceCache[name] = newRes;

    return newRes;
}

} // namespace JzRE
```

### 4.2. `JzTexture` Example

```cpp
// JzTexture.h
#include "JzRE/Resource/JzResource.h"
#include "JzRHITexture.h" // Assuming RHI texture exists

class JzTexture : public JzResource {
public:
    JzTexture(const String& name);
    virtual bool Load() override;
    virtual void Unload() override;
    JzRHITexture* GetRHITexture() const { return m_rhiTexture.get(); }
private:
    std::unique_ptr<JzRHITexture> m_rhiTexture;
};

// JzTextureFactory.h
#include "JzResourceFactory.h"

class JzTextureFactory : public JzResourceFactory {
public:
    virtual JzResource* Create(const String& name) override {
        return new JzTexture(name);
    }
};
```

## 5. Future Work

-   **Resource Dependencies**: Implement a system where resources can depend on others (e.g., a `JzMaterial` depending on `JzShader` and `JzTexture`). Loading a material would automatically trigger the loading of its dependencies.
-   **Hot Reloading**: Monitor resource files on disk and automatically reload them in the editor when they change, allowing for rapid iteration.
-   **Streaming**: For very large resources like textures or models, implement streaming to load parts of the resource on demand.
