#pragma once

#include "JzRHITypes.h"

namespace JzRE {

/**
 * RHI资源基类
 */
class JzRHIResource {
public:
    JzRHIResource(const String &debugName = "") :
        debugName(debugName) { }
    virtual ~JzRHIResource() = default;

    const String &GetDebugName() const
    {
        return debugName;
    }
    void SetDebugName(const String &name)
    {
        debugName = name;
    }

protected:
    String debugName;
};

/**
 * RHI缓冲区接口
 */
class JzRHIBuffer : public JzRHIResource {
public:
    JzRHIBuffer(const BufferDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHIBuffer() = default;

    virtual void  UpdateData(const void *data, Size size, Size offset = 0) = 0;
    virtual void *MapBuffer()                                              = 0;
    virtual void  UnmapBuffer()                                            = 0;

    JzEBufferType GetType() const
    {
        return desc.type;
    }
    JzEBufferUsage GetUsage() const
    {
        return desc.usage;
    }
    Size GetSize() const
    {
        return desc.size;
    }

protected:
    JzBufferDesc desc;
};

/**
 * RHI纹理接口
 */
class JzRHITexture : public JzRHIResource {
public:
    JzRHITexture(const JzTextureDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHITexture() = default;

    virtual void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) = 0;
    virtual void GenerateMipmaps()                                                  = 0;

    JzETextureType GetType() const
    {
        return desc.type;
    }
    JzETextureFormat GetFormat() const
    {
        return desc.format;
    }
    U32 GetWidth() const
    {
        return desc.width;
    }
    U32 GetHeight() const
    {
        return desc.height;
    }
    U32 GetDepth() const
    {
        return desc.depth;
    }
    U32 GetMipLevels() const
    {
        return desc.mipLevels;
    }

protected:
    JzTextureDesc desc;
};

/**
 * RHI着色器接口
 */
class JzRHIShader : public JzRHIResource {
public:
    JzRHIShader(const ShaderDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHIShader() = default;

    JzEShaderType GetType() const
    {
        return desc.type;
    }
    const String &GetSource() const
    {
        return desc.source;
    }
    const String &GetEntryPoint() const
    {
        return desc.entryPoint;
    }

protected:
    JzShaderDesc desc;
};

/**
 * RHI渲染管线接口
 */
class JzRHIPipeline : public JzRHIResource {
public:
    JzRHIPipeline(const JzPipelineDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHIPipeline() = default;

    const JzRenderState &GetRenderState() const
    {
        return desc.renderState;
    }

protected:
    JzPipelineDesc desc;
};

/**
 * RHI帧缓冲接口
 */
class JzRHIFramebuffer : public JzRHIResource {
public:
    JzRHIFramebuffer(const String &debugName = "") :
        JzRHIResource(debugName) { }
    virtual ~JzRHIFramebuffer() = default;

    virtual void AttachColorTexture(std::shared_ptr<JzRHITexture> texture, U32 attachmentIndex = 0) = 0;
    virtual void AttachDepthTexture(std::shared_ptr<JzRHITexture> texture)                          = 0;
    virtual void AttachDepthStencilTexture(std::shared_ptr<JzRHITexture> texture)                   = 0;
    virtual Bool IsComplete() const                                                                 = 0;
};

/**
 * RHI顶点数组对象接口
 */
class JzRHIVertexArray : public JzRHIResource {
public:
    JzRHIVertexArray(const String &debugName = "") :
        JzRHIResource(debugName) { }
    virtual ~JzRHIVertexArray() = default;

    virtual void BindVertexBuffer(std::shared_ptr<JzRHIBuffer> buffer, U32 binding = 0) = 0;
    virtual void BindIndexBuffer(std::shared_ptr<JzRHIBuffer> buffer)                   = 0;
    virtual void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset)        = 0;
};

} // namespace JzRE