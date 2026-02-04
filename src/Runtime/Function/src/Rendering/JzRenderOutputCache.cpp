/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Rendering/JzRenderOutputCache.h"

namespace JzRE {

JzRenderOutput *JzRenderOutputCache::GetOrCreate(const String                   &name,
                                                 std::shared_ptr<JzRenderOutput> output)
{
    if (name.empty()) {
        return nullptr;
    }

    auto it = m_outputs.find(name);
    if (it == m_outputs.end()) {
        if (!output) {
            return nullptr;
        }
        m_outputs.emplace(name, std::move(output));
        return m_outputs[name].get();
    }

    return it->second.get();
}

JzRenderOutput *JzRenderOutputCache::Get(const String &name) const
{
    auto it = m_outputs.find(name);
    if (it == m_outputs.end()) {
        return nullptr;
    }

    return it->second.get();
}

void JzRenderOutputCache::Update(const String &name, std::shared_ptr<JzRenderOutput> output)
{
    if (name.empty() || !output) {
        return;
    }

    m_outputs[name] = std::move(output);
}

void JzRenderOutputCache::Remove(const String &name)
{
    m_outputs.erase(name);
}

void JzRenderOutputCache::Clear()
{
    m_outputs.clear();
}

} // namespace JzRE
