/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <mutex>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzResource.h"
#include "JzRE/Runtime/Resource/JzResourceFactory.h"

namespace JzRE {

/**
 * @brief JzRE resource manager
 */
class JzResourceManager {
public:
    /**
     * @brief Registers a factory for a given resource type T
     *
     * @tparam T
     *
     * @param factory
     */
    template <typename T>
    void RegisterFactory(std::unique_ptr<JzResourceFactory> factory);

    /**
     * @brief Get the Resource object
     *
     * @tparam T
     *
     * @param name
     *
     * @return std::shared_ptr<T>
     */
    template <typename T>
    std::shared_ptr<T> GetResource(const String &name);

    /**
     * @brief Update state of async load task, called every frame to process async queue, etc.
     */
    void Update();

    /**
     * @brief Cleans up expired weak_ptrs from the cache.
     */
    void UnloadUnusedResources();

    /**
     * @brief Add a search path
     *
     * @param path
     */
    void AddSearchPath(const String &path);

    /**
     * @brief Find full path
     *
     * @param relativePath
     *
     * @return String
     */
    String FindFullPath(const String &relativePath);

private:
    std::unordered_map<std::type_index, std::unique_ptr<JzResourceFactory>> m_factories;
    std::unordered_map<String, std::weak_ptr<JzResource>>                   m_resourceCache;
    std::vector<String>                                                     m_searchPaths;
    std::mutex                                                              m_cacheMutex;

    // TODO: Add async loading members (queue, threads, etc.)
    // std::queue<AsyncLoadRequest> m_asyncLoadQueue;
};

} // namespace JzRE

#include "JzRE/Runtime/Resource/JzResourceManager.inl" // IWYU pragma: keep