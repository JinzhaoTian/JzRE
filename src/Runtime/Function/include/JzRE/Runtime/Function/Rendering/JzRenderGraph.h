/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {

class JzDevice;

enum class JzRGUsage : U8 {
    Read,
    Write,
    ReadWrite,
};

struct JzRGTexture {
    U32 id = 0;
};

struct JzRGBuffer {
    U32 id = 0;
};

struct JzRGTextureDesc {
    JzIVec2                  size{0, 0};
    JzETextureResourceFormat format    = JzETextureResourceFormat::RGBA8;
    Bool                     transient = true;
    String                   name;
};

struct JzRGBufferDesc {
    size_t                  size      = 0;
    JzEGPUBufferObjectType  type      = JzEGPUBufferObjectType::Vertex;
    JzEGPUBufferObjectUsage usage     = JzEGPUBufferObjectUsage::StaticDraw;
    Bool                    transient = true;
    String                  name;
};

enum class JzRGResourceType : U8 {
    Texture,
    Buffer,
};

struct JzRGTransition {
    JzRGResourceType type;
    U32              id     = 0;
    JzRGUsage        before = JzRGUsage::Read;
    JzRGUsage        after  = JzRGUsage::Read;
};

/**
 * @brief Runtime execution context for one RenderGraph pass.
 */
struct JzRGPassContext {
    JzDevice                               &device;
    JzIVec2                                 viewport{0, 0};
    JzRGTexture                             colorHandle{};
    JzRGTexture                             depthHandle{};
    std::shared_ptr<JzGPUFramebufferObject> framebuffer;
    std::shared_ptr<JzGPUTextureObject>     colorTexture;
    std::shared_ptr<JzGPUTextureObject>     depthTexture;
};

class JzRGBuilder {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~JzRGBuilder() = default;

    /**
     * @brief Declare a read usage for a texture.
     */
    virtual JzRGTexture Read(JzRGTexture tex, JzRGUsage usage = JzRGUsage::Read) = 0;

    /**
     * @brief Declare a write usage for a texture.
     */
    virtual JzRGTexture Write(JzRGTexture tex, JzRGUsage usage = JzRGUsage::Write) = 0;

    /**
     * @brief Set render target attachments for this pass.
     */
    virtual void SetRenderTarget(JzRGTexture color, JzRGTexture depth = {}) = 0;

    /**
     * @brief Set viewport size for this pass.
     */
    virtual void SetViewport(JzIVec2 size) = 0;
};

/**
 * @brief RenderGraph pass description (phase 1).
 */
struct JzRGPassDesc {
    String                                       name;
    std::function<Bool()>                        enabledExecute = nullptr;
    std::function<void(JzRGBuilder &)>           setup;
    std::function<void(const JzRGPassContext &)> execute;
};

/**
 * @brief Minimal RenderGraph provides ordered pass execution without resource
 * tracking. It is a stepping stone toward a full render graph.
 */
class JzRenderGraph {
public:
    using TransitionCallback = std::function<void(const JzRGPassDesc &,
                                                  const std::vector<JzRGTransition> &)>;

    JzRenderGraph();

    /**
     * @brief Add a pass to the graph.
     *
     * @param desc Pass description
     */
    void AddPass(JzRGPassDesc desc);

    /**
     * @brief Create a logical texture resource.
     *
     * @param desc Texture description
     * @return Texture handle
     */
    JzRGTexture CreateTexture(const JzRGTextureDesc &desc);

    /**
     * @brief Create a logical buffer resource.
     *
     * @param desc Buffer description
     * @return Buffer handle
     */
    JzRGBuffer CreateBuffer(const JzRGBufferDesc &desc);

    /**
     * @brief Compile the graph (phase 1: no-op).
     */
    void Compile();

    /**
     * @brief Execute all passes in order.
     */
    void Execute(JzDevice &device);

    /**
     * @brief Clear all passes for the next frame.
     */
    void Reset();

    /**
     * @brief Dump current graph state to a markdown file.
     *
     * @param path Output file path
     */
    void DumpGraph(const String &path) const;

    /**
     * @brief Bind an external texture resource to a logical texture.
     */
    void BindTexture(JzRGTexture tex, std::shared_ptr<JzGPUTextureObject> resource);

    /**
     * @brief Bind an external buffer resource to a logical buffer.
     */
    void BindBuffer(JzRGBuffer buffer, std::shared_ptr<JzGPUBufferObject> resource);

    /**
     * @brief Bind an external framebuffer for a logical color/depth pair.
     */
    void BindRenderTarget(JzRGTexture color, JzRGTexture depth,
                          std::shared_ptr<JzGPUFramebufferObject> framebuffer);

    /**
     * @brief Get the bound GPU texture resource for a logical texture.
     */
    std::shared_ptr<JzGPUTextureObject> GetTextureResource(JzRGTexture tex) const;

    /**
     * @brief Get the bound GPU buffer resource for a logical buffer.
     */
    std::shared_ptr<JzGPUBufferObject> GetBufferResource(JzRGBuffer buffer) const;

    /**
     * @brief Set transition callback (optional).
     *
     * This can be used to insert backend-specific barriers/state changes.
     */
    void SetTransitionCallback(TransitionCallback callback);

    /**
     * @brief Set allocator callbacks for logical resources.
     */
    void SetTextureAllocator(std::function<std::shared_ptr<JzGPUTextureObject>(const JzRGTextureDesc &)> allocator);
    void SetBufferAllocator(std::function<std::shared_ptr<JzGPUBufferObject>(const JzRGBufferDesc &)> allocator);

private:
    struct JzRGResourceUsage {
        JzRGResourceType type;
        U32              id    = 0;
        JzRGUsage        usage = JzRGUsage::Read;
    };

    struct JzRGPassData {
        JzRGPassDesc                   desc;
        std::vector<JzRGResourceUsage> usages;
        std::vector<JzRGTransition>    transitions;
        JzRGTexture                    colorTarget;
        JzRGTexture                    depthTarget;
        JzIVec2                        viewport{0, 0};
    };

    class JzRGBuilderImpl final : public JzRGBuilder {
    public:
        explicit JzRGBuilderImpl(JzRenderGraph &graph) :
            m_graph(graph) { }

        void SetActivePassIndex(size_t index)
        {
            m_activePass = index;
        }

        JzRGTexture Read(JzRGTexture tex, JzRGUsage usage) override;
        JzRGTexture Write(JzRGTexture tex, JzRGUsage usage) override;
        void        SetRenderTarget(JzRGTexture color, JzRGTexture depth) override;
        void        SetViewport(JzIVec2 size) override;

    private:
        JzRenderGraph &m_graph;
        size_t         m_activePass = static_cast<size_t>(-1);
    };

    std::vector<JzRGPassData>                                        m_passes;
    std::vector<JzRGTextureDesc>                                     m_textures;
    std::vector<JzRGBufferDesc>                                      m_buffers;
    std::vector<std::shared_ptr<JzGPUTextureObject>>                 m_textureResources;
    std::vector<std::shared_ptr<JzGPUBufferObject>>                  m_bufferResources;
    std::unordered_map<U64, std::shared_ptr<JzGPUFramebufferObject>> m_boundRenderTargets;
    std::unordered_map<U64, std::shared_ptr<JzGPUFramebufferObject>> m_framebufferPool;
    TransitionCallback                                               m_transitionCallback;
    std::vector<size_t>                                              m_executionOrder;
    Bool                                                             m_hasCycle = false;
    JzRGBuilderImpl                                                  m_builder;

    void BuildTransitions(const std::vector<size_t> &order);
    void AllocateResources();

    struct JzRGTexturePoolEntry {
        JzRGTextureDesc                     desc;
        std::shared_ptr<JzGPUTextureObject> resource;
        Bool                                inUse = false;
    };

    struct JzRGBufferPoolEntry {
        JzRGBufferDesc                     desc;
        std::shared_ptr<JzGPUBufferObject> resource;
        Bool                               inUse = false;
    };

    std::vector<JzRGTexturePoolEntry> m_texturePool;
    std::vector<JzRGBufferPoolEntry>  m_bufferPool;

    std::function<std::shared_ptr<JzGPUTextureObject>(const JzRGTextureDesc &)> m_textureAllocator;
    std::function<std::shared_ptr<JzGPUBufferObject>(const JzRGBufferDesc &)>   m_bufferAllocator;

    static U64                              BuildRenderTargetKey(U32 colorId, U32 depthId);
    static U64                              BuildFramebufferPoolKey(const std::shared_ptr<JzGPUTextureObject> &color,
                                                                    const std::shared_ptr<JzGPUTextureObject> &depth);
    std::shared_ptr<JzGPUFramebufferObject> ResolveFramebuffer(
        JzDevice &device, const JzRGPassData &pass,
        const std::shared_ptr<JzGPUTextureObject> &colorTexture,
        const std::shared_ptr<JzGPUTextureObject> &depthTexture);
};

} // namespace JzRE
