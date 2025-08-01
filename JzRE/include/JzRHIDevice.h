#pragma once

#include "CommonTypes.h"
#include "JzRHICommandBuffer.h"
#include "JzRHIETypes.h"
#include "JzRHIFramebuffer.h"
#include "JzRHIShader.h"

namespace JzRE {
/**
 * @brief Interface of RHI Device
 */
class JzRHIDevice {
public:
    JzRHIDevice(JzERHIType rhiType) :
        rhiType(rhiType) { }

    virtual ~JzRHIDevice() = default;

    // 设备信息
    JzERHIType GetRHIType() const
    {
        return rhiType;
    }
    virtual String GetDeviceName() const    = 0;
    virtual String GetVendorName() const    = 0;
    virtual String GetDriverVersion() const = 0;

    // 资源创建接口
    virtual std::shared_ptr<JzRHIBuffer>      CreateBuffer(const JzBufferDesc &desc)          = 0;
    virtual std::shared_ptr<JzRHITexture>     CreateTexture(const JzTextureDesc &desc)        = 0;
    virtual std::shared_ptr<JzRHIShader>      CreateShader(const JzShaderDesc &desc)          = 0;
    virtual std::shared_ptr<JzRHIPipeline>    CreatePipeline(const JzPipelineDesc &desc)      = 0;
    virtual std::shared_ptr<JzRHIFramebuffer> CreateFramebuffer(const String &debugName = "") = 0;
    virtual std::shared_ptr<JzRHIVertexArray> CreateVertexArray(const String &debugName = "") = 0;

    // 命令系统
    virtual std::shared_ptr<JzRHICommandBuffer> CreateCommandBuffer(const String &debugName = "")                       = 0;
    virtual void                                ExecuteCommandBuffer(std::shared_ptr<JzRHICommandBuffer> commandBuffer) = 0;

    // 立即渲染模式（与命令缓冲模式互斥）
    virtual void BeginFrame() = 0;
    virtual void EndFrame()   = 0;
    virtual void Present()    = 0;

    // 渲染状态管理
    virtual void SetRenderState(const JzRenderState &state) = 0;
    virtual void SetViewport(const JzViewport &viewport)    = 0;
    virtual void SetScissor(const JzScissorRect &scissor)   = 0;

    // 绘制接口
    virtual void Clear(const JzClearParams &params)             = 0;
    virtual void Draw(const JzDrawParams &params)               = 0;
    virtual void DrawIndexed(const JzDrawIndexedParams &params) = 0;

    // 资源绑定
    virtual void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline)          = 0;
    virtual void BindVertexArray(std::shared_ptr<JzRHIVertexArray> vertexArray) = 0;
    virtual void BindTexture(std::shared_ptr<JzRHITexture> texture, U32 slot)   = 0;
    virtual void BindFramebuffer(std::shared_ptr<JzRHIFramebuffer> framebuffer) = 0;

    // 同步和等待
    virtual void Flush()  = 0;
    virtual void Finish() = 0;

    // 多线程支持
    virtual Bool SupportsMultithreading() const          = 0;
    virtual void MakeContextCurrent(U32 threadIndex = 0) = 0;

protected:
    JzERHIType rhiType;
};
} // namespace JzRE