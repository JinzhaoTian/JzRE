/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzResourceManager.h"
#include <filesystem>

void JzRE::JzResourceManager::Update()
{
    // Process async loading queue in the future
}

void JzRE::JzResourceManager::UnloadUnusedResources()
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    for (auto it = m_resourceCache.begin(); it != m_resourceCache.end();) {
        if (it->second.expired()) {
            it = m_resourceCache.erase(it);
        } else {
            ++it;
        }
    }
}

void JzRE::JzResourceManager::AddSearchPath(const JzRE::String &path)
{
    m_searchPaths.push_back(path);
}

JzRE::String JzRE::JzResourceManager::FindFullPath(const JzRE::String &relativePath)
{
    for (const auto &searchPath : m_searchPaths) {
        String fullPath = searchPath + "/" + relativePath;
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }
    // Return empty string if not found, or log an error
    return "";
}
