/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Rendering/JzRenderGraph.h"

#include <cstdint>
#include <fstream>

#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

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

void JzRenderGraph::Execute(JzDevice &device)
{
    std::vector<size_t> order;
    if (m_executionOrder.empty()) {
        order.resize(m_passes.size());
        for (size_t i = 0; i < m_passes.size(); ++i) {
            order[i] = i;
        }
    } else {
        order = m_executionOrder;
    }

    for (size_t index : order) {
        auto &pass = m_passes[index];
        if (pass.desc.enabledExecute && !pass.desc.enabledExecute()) {
            continue;
        }

        m_builder.SetActivePassIndex(index);

        auto commandList = device.CreateCommandList("RenderGraph_" + pass.desc.name);
        if (!commandList) {
            continue;
        }
        commandList->Begin();

        if (m_transitionCallback && !pass.transitions.empty()) {
            m_transitionCallback(*commandList, pass.desc, pass.transitions);
        }

        if (pass.desc.execute) {
            auto colorTexture = GetTextureResource(pass.colorTarget);
            auto depthTexture = GetTextureResource(pass.depthTarget);
            auto framebuffer  = ResolveFramebuffer(device, pass, colorTexture, depthTexture);

            if (pass.colorTarget.id != 0 || pass.depthTarget.id != 0 || framebuffer) {
                commandList->BindFramebuffer(framebuffer);
            }

            if (pass.viewport.x > 0 && pass.viewport.y > 0) {
                JzViewport viewport;
                viewport.x        = 0.0f;
                viewport.y        = 0.0f;
                viewport.width    = static_cast<F32>(pass.viewport.x);
                viewport.height   = static_cast<F32>(pass.viewport.y);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                commandList->SetViewport(viewport);
            }

            const JzRGPassContext context{
                *commandList,
                pass.viewport,
                pass.colorTarget,
                pass.depthTarget,
                framebuffer,
                colorTexture,
                depthTexture};
            pass.desc.execute(context);
        }

        commandList->End();
        device.ExecuteCommandList(commandList);
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
    m_boundRenderTargets.clear();

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

void JzRenderGraph::BindRenderTarget(JzRGTexture color, JzRGTexture depth,
                                     std::shared_ptr<JzGPUFramebufferObject> framebuffer)
{
    if (color.id == 0 && depth.id == 0) {
        return;
    }

    const U64 key = BuildRenderTargetKey(color.id, depth.id);
    if (!framebuffer) {
        m_boundRenderTargets.erase(key);
        return;
    }

    m_boundRenderTargets[key] = std::move(framebuffer);
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

U64 JzRenderGraph::BuildRenderTargetKey(U32 colorId, U32 depthId)
{
    return (static_cast<U64>(colorId) << 32U) | static_cast<U64>(depthId);
}

U64 JzRenderGraph::BuildFramebufferPoolKey(const std::shared_ptr<JzGPUTextureObject> &color,
                                           const std::shared_ptr<JzGPUTextureObject> &depth)
{
    if (!color && !depth) {
        return 0;
    }

    const U64 colorPtr = static_cast<U64>(reinterpret_cast<std::uintptr_t>(color.get()));
    const U64 depthPtr = static_cast<U64>(reinterpret_cast<std::uintptr_t>(depth.get()));
    return colorPtr ^ (depthPtr + 0x9e3779b97f4a7c15ULL + (colorPtr << 6U) + (colorPtr >> 2U));
}

std::shared_ptr<JzGPUFramebufferObject> JzRenderGraph::ResolveFramebuffer(
    JzDevice &device, const JzRGPassData &pass,
    const std::shared_ptr<JzGPUTextureObject> &colorTexture,
    const std::shared_ptr<JzGPUTextureObject> &depthTexture)
{
    if (pass.colorTarget.id == 0 && pass.depthTarget.id == 0) {
        return nullptr;
    }

    const U64 boundTargetKey = BuildRenderTargetKey(pass.colorTarget.id, pass.depthTarget.id);
    auto      boundTargetIt  = m_boundRenderTargets.find(boundTargetKey);
    if (boundTargetIt != m_boundRenderTargets.end()) {
        return boundTargetIt->second;
    }

    if (!colorTexture && !depthTexture) {
        return nullptr;
    }

    const U64 poolKey = BuildFramebufferPoolKey(colorTexture, depthTexture);
    if (poolKey == 0) {
        return nullptr;
    }

    auto cachedFramebuffer = m_framebufferPool.find(poolKey);
    if (cachedFramebuffer != m_framebufferPool.end()) {
        return cachedFramebuffer->second;
    }

    auto framebuffer = device.CreateFramebuffer("RenderGraph_Framebuffer");
    if (!framebuffer) {
        return nullptr;
    }

    if (colorTexture) {
        framebuffer->AttachColorTexture(colorTexture, 0);
    }
    if (depthTexture) {
        framebuffer->AttachDepthTexture(depthTexture);
    }

    m_framebufferPool[poolKey] = framebuffer;
    return framebuffer;
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
