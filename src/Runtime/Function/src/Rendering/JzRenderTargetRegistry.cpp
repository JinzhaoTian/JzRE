/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Rendering/JzRenderTargetRegistry.h"

#include <algorithm>

namespace JzRE {

JzRenderTargetRegistry::Handle JzRenderTargetRegistry::Register(JzRenderTargetEntry entry)
{
    Handle handle = m_nextHandle++;
    m_entries.emplace_back(handle, std::move(entry));
    return handle;
}

void JzRenderTargetRegistry::Unregister(Handle handle)
{
    auto it = std::find_if(m_entries.begin(), m_entries.end(),
                           [handle](const auto &pair) { return pair.first == handle; });

    if (it != m_entries.end()) {
        m_entries.erase(it);
    }
}

void JzRenderTargetRegistry::UpdateCamera(Handle handle, JzEntity camera)
{
    auto it = std::find_if(m_entries.begin(), m_entries.end(),
                           [handle](const auto &pair) { return pair.first == handle; });

    if (it != m_entries.end()) {
        it->second.camera = camera;
    }
}

std::vector<std::pair<JzRenderTargetRegistry::Handle, JzRenderTargetEntry>> &
JzRenderTargetRegistry::GetEntries()
{
    return m_entries;
}

const std::vector<std::pair<JzRenderTargetRegistry::Handle, JzRenderTargetEntry>> &
JzRenderTargetRegistry::GetEntries() const
{
    return m_entries;
}

} // namespace JzRE
