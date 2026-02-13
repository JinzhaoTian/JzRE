/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Rendering/JzRenderGraph.h"

#include <fstream>

namespace JzRE {

JzRenderGraph::JzRenderGraph() :
    m_builder(*this) { }

void JzRenderGraph::AddPass(JzRGPassDesc desc)
{
    JzRGPassData data;
    data.desc = std::move(desc);
    m_passes.emplace_back(std::move(data));
}

JzRGTexture JzRenderGraph::CreateTexture(const JzRGTextureDesc &desc)
{
    m_textures.emplace_back(desc);
    m_textureResources.emplace_back(nullptr);
    return JzRGTexture{static_cast<U32>(m_textures.size())};
}

JzRGBuffer JzRenderGraph::CreateBuffer(const JzRGBufferDesc &desc)
{
    m_buffers.emplace_back(desc);
    m_bufferResources.emplace_back(nullptr);
    return JzRGBuffer{static_cast<U32>(m_buffers.size())};
}

void JzRenderGraph::Compile()
{
    m_executionOrder.clear();
    m_hasCycle = false;

    const size_t passCount = m_passes.size();
    if (passCount == 0) {
        return;
    }

    for (size_t i = 0; i < passCount; ++i) {
        auto &pass = m_passes[i];
        m_builder.SetActivePassIndex(i);
        if (pass.desc.setup) {
            pass.desc.setup(m_builder);
        }
    }

    std::vector<std::vector<size_t>> edges(passCount);
    std::vector<size_t>              indegree(passCount, 0);

    // Track last writer per resource to build dependencies.
    std::unordered_map<U32, size_t> lastTextureWriter;
    std::unordered_map<U32, size_t> lastBufferWriter;

    for (size_t i = 0; i < passCount; ++i) {
        for (const auto &usage : m_passes[i].usages) {
            if (usage.id == 0) {
                continue;
            }

            if (usage.type == JzRGResourceType::Texture) {
                auto it = lastTextureWriter.find(usage.id);
                if (it != lastTextureWriter.end() && it->second != i) {
                    edges[it->second].push_back(i);
                }

                if (usage.usage != JzRGUsage::Read) {
                    lastTextureWriter[usage.id] = i;
                }
            } else {
                auto it = lastBufferWriter.find(usage.id);
                if (it != lastBufferWriter.end() && it->second != i) {
                    edges[it->second].push_back(i);
                }

                if (usage.usage != JzRGUsage::Read) {
                    lastBufferWriter[usage.id] = i;
                }
            }
        }
    }

    for (size_t u = 0; u < passCount; ++u) {
        for (size_t v : edges[u]) {
            ++indegree[v];
        }
    }

    std::vector<size_t> queue;
    queue.reserve(passCount);
    for (size_t i = 0; i < passCount; ++i) {
        if (indegree[i] == 0) {
            queue.push_back(i);
        }
    }

    size_t qIndex = 0;
    while (qIndex < queue.size()) {
        const size_t u = queue[qIndex++];
        m_executionOrder.push_back(u);
        for (size_t v : edges[u]) {
            if (--indegree[v] == 0) {
                queue.push_back(v);
            }
        }
    }

    if (m_executionOrder.size() != passCount) {
        m_hasCycle = true;
        m_executionOrder.clear();
    }

    std::vector<size_t> order;
    if (!m_executionOrder.empty()) {
        order = m_executionOrder;
    } else {
        order.resize(passCount);
        for (size_t i = 0; i < passCount; ++i) {
            order[i] = i;
        }
    }

    BuildTransitions(order);
    AllocateResources();
}

void JzRenderGraph::Execute()
{
    if (m_executionOrder.empty()) {
        for (size_t i = 0; i < m_passes.size(); ++i) {
            auto &pass = m_passes[i];
            if (pass.desc.enabledExecute && !pass.desc.enabledExecute()) {
                continue;
            }

            m_builder.SetActivePassIndex(i);

            if (m_transitionCallback && !pass.transitions.empty()) {
                m_transitionCallback(pass.desc, pass.transitions);
            }

            if (pass.desc.execute) {
                pass.desc.execute();
            }
        }
        return;
    }

    for (size_t index : m_executionOrder) {
        auto &pass = m_passes[index];
        if (pass.desc.enabledExecute && !pass.desc.enabledExecute()) {
            continue;
        }

        m_builder.SetActivePassIndex(index);

        if (m_transitionCallback && !pass.transitions.empty()) {
            m_transitionCallback(pass.desc, pass.transitions);
        }

        if (pass.desc.execute) {
            pass.desc.execute();
        }
    }
}

void JzRenderGraph::Reset()
{
    m_passes.clear();
    m_textures.clear();
    m_buffers.clear();
    m_textureResources.clear();
    m_bufferResources.clear();
    m_executionOrder.clear();
    m_hasCycle = false;

    for (auto &entry : m_texturePool) {
        entry.inUse = false;
    }
    for (auto &entry : m_bufferPool) {
        entry.inUse = false;
    }
}

void JzRenderGraph::DumpGraph(const String &path) const
{
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        return;
    }

    out << "# RenderGraph Dump\n\n";
    out << "## Passes\n";
    for (size_t i = 0; i < m_passes.size(); ++i) {
        const auto &pass = m_passes[i];
        out << "- [" << i << "] " << pass.desc.name << "\n";
    }

    out << "\n## Execution Order\n";
    if (m_executionOrder.empty()) {
        out << "- (default order)\n";
        for (size_t i = 0; i < m_passes.size(); ++i) {
            out << "  - " << m_passes[i].desc.name << "\n";
        }
    } else {
        for (size_t index : m_executionOrder) {
            out << "- " << m_passes[index].desc.name << "\n";
        }
    }

    out << "\n## Resources\n";
    out << "### Textures\n";
    for (size_t i = 0; i < m_textures.size(); ++i) {
        const auto &tex = m_textures[i];
        out << "- [" << (i + 1) << "] " << tex.name << " (" << tex.size.x << "x"
            << tex.size.y << ")\n";
    }

    out << "\n### Buffers\n";
    for (size_t i = 0; i < m_buffers.size(); ++i) {
        const auto &buf = m_buffers[i];
        out << "- [" << (i + 1) << "] " << buf.name << " (size=" << buf.size << ")\n";
    }

    out << "\n## Transitions\n";
    for (size_t i = 0; i < m_passes.size(); ++i) {
        const auto &pass = m_passes[i];
        if (pass.transitions.empty()) {
            continue;
        }
        out << "- " << pass.desc.name << "\n";
        for (const auto &t : pass.transitions) {
            out << "  - " << (t.type == JzRGResourceType::Texture ? "Texture" : "Buffer")
                << " #" << t.id << "\n";
        }
    }
}

void JzRenderGraph::BindTexture(JzRGTexture tex, std::shared_ptr<JzGPUTextureObject> resource)
{
    if (tex.id == 0 || tex.id > m_textureResources.size()) {
        return;
    }

    m_textureResources[tex.id - 1] = std::move(resource);
}

void JzRenderGraph::BindBuffer(JzRGBuffer buffer, std::shared_ptr<JzGPUBufferObject> resource)
{
    if (buffer.id == 0 || buffer.id > m_bufferResources.size()) {
        return;
    }

    m_bufferResources[buffer.id - 1] = std::move(resource);
}

std::shared_ptr<JzGPUTextureObject> JzRenderGraph::GetTextureResource(JzRGTexture tex) const
{
    if (tex.id == 0 || tex.id > m_textureResources.size()) {
        return nullptr;
    }

    return m_textureResources[tex.id - 1];
}

std::shared_ptr<JzGPUBufferObject> JzRenderGraph::GetBufferResource(JzRGBuffer buffer) const
{
    if (buffer.id == 0 || buffer.id > m_bufferResources.size()) {
        return nullptr;
    }

    return m_bufferResources[buffer.id - 1];
}

void JzRenderGraph::SetTransitionCallback(TransitionCallback callback)
{
    m_transitionCallback = std::move(callback);
}

void JzRenderGraph::SetTextureAllocator(
    std::function<std::shared_ptr<JzGPUTextureObject>(const JzRGTextureDesc &)> allocator)
{
    m_textureAllocator = std::move(allocator);
}

void JzRenderGraph::SetBufferAllocator(
    std::function<std::shared_ptr<JzGPUBufferObject>(const JzRGBufferDesc &)> allocator)
{
    m_bufferAllocator = std::move(allocator);
}

JzRGTexture JzRenderGraph::JzRGBuilderImpl::Read(JzRGTexture tex, JzRGUsage usage)
{
    if (m_activePass >= m_graph.m_passes.size()) {
        return tex;
    }

    m_graph.m_passes[m_activePass].usages.push_back(
        {JzRGResourceType::Texture, tex.id, usage});
    return tex;
}

JzRGTexture JzRenderGraph::JzRGBuilderImpl::Write(JzRGTexture tex, JzRGUsage usage)
{
    if (m_activePass >= m_graph.m_passes.size()) {
        return tex;
    }

    m_graph.m_passes[m_activePass].usages.push_back(
        {JzRGResourceType::Texture, tex.id, usage});
    return tex;
}

void JzRenderGraph::JzRGBuilderImpl::SetRenderTarget(JzRGTexture color, JzRGTexture depth)
{
    if (m_activePass >= m_graph.m_passes.size()) {
        return;
    }

    auto &pass       = m_graph.m_passes[m_activePass];
    pass.colorTarget = color;
    pass.depthTarget = depth;
}

void JzRenderGraph::JzRGBuilderImpl::SetViewport(JzIVec2 size)
{
    if (m_activePass >= m_graph.m_passes.size()) {
        return;
    }

    m_graph.m_passes[m_activePass].viewport = size;
}

void JzRenderGraph::BuildTransitions(const std::vector<size_t> &order)
{
    std::unordered_map<U32, JzRGUsage> lastTextureUsage;
    std::unordered_map<U32, JzRGUsage> lastBufferUsage;

    for (size_t passIndex : order) {
        auto &pass = m_passes[passIndex];
        pass.transitions.clear();

        for (const auto &usage : pass.usages) {
            if (usage.id == 0) {
                continue;
            }

            if (usage.type == JzRGResourceType::Texture) {
                const auto      last   = lastTextureUsage.find(usage.id);
                const JzRGUsage before = last != lastTextureUsage.end() ? last->second : usage.usage;
                if (before != usage.usage) {
                    pass.transitions.push_back(
                        {JzRGResourceType::Texture, usage.id, before, usage.usage});
                }
                lastTextureUsage[usage.id] = usage.usage;
            } else {
                const auto      last   = lastBufferUsage.find(usage.id);
                const JzRGUsage before = last != lastBufferUsage.end() ? last->second : usage.usage;
                if (before != usage.usage) {
                    pass.transitions.push_back(
                        {JzRGResourceType::Buffer, usage.id, before, usage.usage});
                }
                lastBufferUsage[usage.id] = usage.usage;
            }
        }
    }
}

void JzRenderGraph::AllocateResources()
{
    if (m_textureResources.size() != m_textures.size()) {
        m_textureResources.resize(m_textures.size());
    }
    if (m_bufferResources.size() != m_buffers.size()) {
        m_bufferResources.resize(m_buffers.size());
    }

    for (size_t i = 0; i < m_textures.size(); ++i) {
        if (m_textureResources[i]) {
            continue;
        }

        const auto &desc = m_textures[i];
        if (desc.transient) {
            for (auto &entry : m_texturePool) {
                const Bool match = entry.desc.size.x == desc.size.x && entry.desc.size.y == desc.size.y && entry.desc.format == desc.format;
                if (match && !entry.inUse) {
                    entry.inUse           = true;
                    m_textureResources[i] = entry.resource;
                    break;
                }
            }
        }

        if (!m_textureResources[i] && m_textureAllocator) {
            auto resource = m_textureAllocator(desc);
            if (resource) {
                m_textureResources[i] = resource;
                if (desc.transient) {
                    m_texturePool.push_back({desc, resource, true});
                }
            }
        }
    }

    for (size_t i = 0; i < m_buffers.size(); ++i) {
        if (m_bufferResources[i]) {
            continue;
        }

        const auto &desc = m_buffers[i];
        if (desc.transient) {
            for (auto &entry : m_bufferPool) {
                const Bool match = entry.desc.size == desc.size && entry.desc.type == desc.type && entry.desc.usage == desc.usage;
                if (match && !entry.inUse) {
                    entry.inUse          = true;
                    m_bufferResources[i] = entry.resource;
                    break;
                }
            }
        }

        if (!m_bufferResources[i] && m_bufferAllocator) {
            auto resource = m_bufferAllocator(desc);
            if (resource) {
                m_bufferResources[i] = resource;
                if (desc.transient) {
                    m_bufferPool.push_back({desc, resource, true});
                }
            }
        }
    }
}

} // namespace JzRE
