/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Core/JzLogger.h"

#include <algorithm>
#include <filesystem>

namespace JzRE {

JzAssetManager::JzAssetManager(const JzAssetManagerConfig &config) :
    m_config(config) { }

JzAssetManager::~JzAssetManager()
{
    if (m_initialized) {
        Shutdown();
    }
}

void JzAssetManager::Initialize()
{
    if (m_initialized) {
        return;
    }

    JzRE_LOG_INFO("JzAssetManager: Initializing with {}MB cache, {} worker threads",
                  m_config.maxCacheMemoryMB, m_config.asyncWorkerCount);

    // Create LRU cache manager
    m_lruCache = std::make_unique<JzLRUCacheManager>(m_config.maxCacheMemoryMB);

    // Create thread pool for async loading
    if (m_config.asyncWorkerCount > 0) {
        m_loadThreadPool = std::make_unique<JzThreadPool>(m_config.asyncWorkerCount);
    }

    // Initialize registries for common types will happen lazily via GetOrCreateRegistry

    m_initialized = true;
    JzRE_LOG_INFO("JzAssetManager: Initialized successfully");
}

void JzAssetManager::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    JzRE_LOG_INFO("JzAssetManager: Shutting down...");

    // Stop thread pool
    if (m_loadThreadPool) {
        m_loadThreadPool->Stop();
        m_loadThreadPool.reset();
    }

    // Clear all registries
    {
        std::unique_lock lock(m_registryMutex);
        m_registries.clear();
    }

    // Clear factories
    {
        std::lock_guard lock(m_factoryMutex);
        m_factories.clear();
    }

    // Clear callbacks
    {
        std::lock_guard lock(m_callbackMutex);
        m_callbacks.clear();
    }

    // Clear queues
    {
        std::lock_guard lock(m_loadQueueMutex);
        while (!m_loadQueue.empty()) {
            m_loadQueue.pop();
        }
    }

    {
        std::lock_guard lock(m_resultQueueMutex);
        while (!m_resultQueue.empty()) {
            m_resultQueue.pop();
        }
    }

    // Clear LRU cache
    if (m_lruCache) {
        m_lruCache->Clear();
        m_lruCache.reset();
    }

    // Clear search paths
    {
        std::lock_guard lock(m_searchPathMutex);
        m_searchPaths.clear();
    }

    m_initialized = false;
    JzRE_LOG_INFO("JzAssetManager: Shutdown complete");
}

void JzAssetManager::CancelLoad(JzAssetId id)
{
    // Remove callback
    {
        std::lock_guard lock(m_callbackMutex);
        m_callbacks.erase(id);
    }

    // Note: We can't easily cancel in-flight loads
    // The request will complete but the callback won't fire
}

void JzAssetManager::Update()
{
    if (!m_initialized) {
        return;
    }

    // Process async load queue
    ProcessAsyncQueue();

    // Process completed results
    ProcessResults();

    // Check for LRU eviction
    if (m_lruCache && m_lruCache->GetMemoryUsageRatio() > m_config.lruEvictionThreshold) {
        Size targetMemoryMB =
            static_cast<Size>(static_cast<F32>(m_config.maxCacheMemoryMB) * m_config.lruEvictionThreshold * 0.9f);
        EvictToTarget(targetMemoryMB);
    }
}

void JzAssetManager::EvictToTarget(Size targetMemoryMB)
{
    if (!m_lruCache) {
        return;
    }

    // Get eviction candidates
    auto candidates = m_lruCache->GetEvictionCandidates(targetMemoryMB);

    for (const auto &id : candidates) {
        // Find the registry that contains this asset
        // This is a bit awkward since we don't know the type
        // For now, just remove from LRU cache
        // The actual unload would need type information

        m_lruCache->Remove(id);

        JzRE_LOG_DEBUG("JzAssetManager: Evicted asset from LRU cache");
    }
}

void JzAssetManager::UnloadUnused()
{
    // Iterate all registries and find assets with 0 ref count
    std::shared_lock lock(m_registryMutex);

    for (auto &[typeIdx, entry] : m_registries) {
        // We can't easily iterate type-erased registries
        // This would need a visitor pattern or base class
        // For now, this is a placeholder
    }
}

void JzAssetManager::AddSearchPath(const String &path)
{
    std::lock_guard lock(m_searchPathMutex);

    // Avoid duplicates
    auto it = std::find(m_searchPaths.begin(), m_searchPaths.end(), path);
    if (it == m_searchPaths.end()) {
        m_searchPaths.push_back(path);
        JzRE_LOG_DEBUG("JzAssetManager: Added search path '{}'", path);
    }
}

String JzAssetManager::FindFullPath(const String &relativePath) const
{
    namespace fs                            = std::filesystem;
    const auto TryResolveShaderManifestPath = [](const fs::path &basePath) -> String {
        if (basePath.extension() == ".jzshader" && fs::exists(basePath)) {
            return basePath.string();
        }

        if (!basePath.extension().empty()) {
            return "";
        }

        fs::path shaderManifest  = basePath;
        shaderManifest          += ".jzshader";
        if (fs::exists(shaderManifest)) {
            return shaderManifest.string();
        }

        return "";
    };

    // If path is absolute and exists, return it
    fs::path p(relativePath);
    if (p.is_absolute() && fs::exists(p)) {
        return relativePath;
    }
    if (p.is_absolute()) {
        auto shaderManifestPath = TryResolveShaderManifestPath(p);
        if (!shaderManifestPath.empty()) {
            return shaderManifestPath;
        }
    }

    // Search in registered paths
    std::lock_guard lock(m_searchPathMutex);

    for (const auto &searchPath : m_searchPaths) {
        fs::path fullPath = fs::path(searchPath) / relativePath;
        if (fs::exists(fullPath)) {
            return fullPath.string();
        }

        auto shaderManifestPath = TryResolveShaderManifestPath(fullPath);
        if (!shaderManifestPath.empty()) {
            return shaderManifestPath;
        }
    }

    // Try relative to current directory
    if (fs::exists(p)) {
        return fs::absolute(p).string();
    }
    auto shaderManifestPath = TryResolveShaderManifestPath(p);
    if (!shaderManifestPath.empty()) {
        return fs::absolute(shaderManifestPath).string();
    }

    return ""; // Not found
}

Size JzAssetManager::GetTotalMemoryUsage() const
{
    if (m_lruCache) {
        return m_lruCache->GetCurrentMemoryUsage();
    }
    return 0;
}

Size JzAssetManager::GetPendingLoadCount() const
{
    std::lock_guard lock(m_loadQueueMutex);
    return m_loadQueue.size();
}

void JzAssetManager::ProcessAsyncQueue()
{
    if (!m_loadThreadPool) {
        return;
    }

    // Process up to a batch of requests per frame
    constexpr Size MaxBatchSize = 4;
    Size           processed    = 0;

    while (processed < MaxBatchSize) {
        JzAssetLoadRequest request;

        {
            std::lock_guard lock(m_loadQueueMutex);
            if (m_loadQueue.empty()) {
                break;
            }
            request = m_loadQueue.top();
            m_loadQueue.pop();
        }

        // Submit to thread pool
        // Note: We need type information to load properly
        // For now, this is a simplified version that just records completion

        m_loadThreadPool->Submit([this, request]() {
            // The actual loading depends on the type
            // This would need a type dispatch mechanism

            JzAssetLoadResult result;
            result.id           = request.id;
            result.typeIndex    = request.typeIndex;
            result.success      = false; // Placeholder - actual loading handled by typed methods
            result.errorMessage = "Async load not fully implemented";

            {
                std::lock_guard lock(m_resultQueueMutex);
                m_resultQueue.push(result);
            }
        });

        ++processed;
    }
}

void JzAssetManager::ProcessResults()
{
    while (true) {
        JzAssetLoadResult result;

        {
            std::lock_guard lock(m_resultQueueMutex);
            if (m_resultQueue.empty()) {
                break;
            }
            result = m_resultQueue.front();
            m_resultQueue.pop();
        }

        // Find and invoke callback
        {
            std::lock_guard lock(m_callbackMutex);
            auto            it = m_callbacks.find(result.id);
            if (it != m_callbacks.end()) {
                it->second.callback(result.success);
                m_callbacks.erase(it);
            }
        }
    }
}

} // namespace JzRE
