#pragma once

#include "RHITypes.h"

namespace JzRE {

/**
 * RHI资源基类
 */
class RHIResource {
public:
    RHIResource(const String& debugName = "") : debugName(debugName) {}
    virtual ~RHIResource() = default;

    const String& GetDebugName() const { return debugName; }
    void SetDebugName(const String& name) { debugName = name; }

protected:
    String debugName;
};

/**
 * RHI缓冲区接口
 */
class RHIBuffer : public RHIResource {
public:
    RHIBuffer(const BufferDesc& desc) : RHIResource(desc.debugName), desc(desc) {}
    virtual ~RHIBuffer() = default;

    virtual void UpdateData(const void* data, Size size, Size offset = 0) = 0;
    virtual void* MapBuffer() = 0;
    virtual void UnmapBuffer() = 0;
    
    EBufferType GetType() const { return desc.type; }
    EBufferUsage GetUsage() const { return desc.usage; }
    Size GetSize() const { return desc.size; }

protected:
    BufferDesc desc;
};

/**
 * RHI纹理接口
 */
class RHITexture : public RHIResource {
public:
    RHITexture(const TextureDesc& desc) : RHIResource(desc.debugName), desc(desc) {}
    virtual ~RHITexture() = default;

    virtual void UpdateData(const void* data, U32 mipLevel = 0, U32 arrayIndex = 0) = 0;
    virtual void GenerateMipmaps() = 0;
    
    ETextureType GetType() const { return desc.type; }
    ETextureFormat GetFormat() const { return desc.format; }
    U32 GetWidth() const { return desc.width; }
    U32 GetHeight() const { return desc.height; }
    U32 GetDepth() const { return desc.depth; }
    U32 GetMipLevels() const { return desc.mipLevels; }

protected:
    TextureDesc desc;
};

/**
 * RHI着色器接口
 */
class RHIShader : public RHIResource {
public:
    RHIShader(const ShaderDesc& desc) : RHIResource(desc.debugName), desc(desc) {}
    virtual ~RHIShader() = default;

    EShaderType GetType() const { return desc.type; }
    const String& GetSource() const { return desc.source; }
    const String& GetEntryPoint() const { return desc.entryPoint; }

protected:
    ShaderDesc desc;
};

/**
 * RHI渲染管线接口
 */
class RHIPipeline : public RHIResource {
public:
    RHIPipeline(const PipelineDesc& desc) : RHIResource(desc.debugName), desc(desc) {}
    virtual ~RHIPipeline() = default;

    const RenderState& GetRenderState() const { return desc.renderState; }

protected:
    PipelineDesc desc;
};

/**
 * RHI帧缓冲接口
 */
class RHIFramebuffer : public RHIResource {
public:
    RHIFramebuffer(const String& debugName = "") : RHIResource(debugName) {}
    virtual ~RHIFramebuffer() = default;

    virtual void AttachColorTexture(std::shared_ptr<RHITexture> texture, U32 attachmentIndex = 0) = 0;
    virtual void AttachDepthTexture(std::shared_ptr<RHITexture> texture) = 0;
    virtual void AttachDepthStencilTexture(std::shared_ptr<RHITexture> texture) = 0;
    virtual Bool IsComplete() const = 0;
};

/**
 * RHI顶点数组对象接口
 */
class RHIVertexArray : public RHIResource {
public:
    RHIVertexArray(const String& debugName = "") : RHIResource(debugName) {}
    virtual ~RHIVertexArray() = default;

    virtual void BindVertexBuffer(std::shared_ptr<RHIBuffer> buffer, U32 binding = 0) = 0;
    virtual void BindIndexBuffer(std::shared_ptr<RHIBuffer> buffer) = 0;
    virtual void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) = 0;
};

} // namespace JzRE 