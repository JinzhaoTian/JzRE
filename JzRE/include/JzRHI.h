#pragma once

#include "JzRHICommand.h"
#include "JzRHIResource.h"
#include "JzRHITypes.h"

namespace JzRE {

/**
 * RHI设备接口
 * 提供跨平台的图形设备抽象
 */
class JzRHIDevice {
public:
    JzRHIDevice(JzERHIType rhiType) :
        rhiType(rhiType) { }
    virtual ~RHIDevice() = default;

    // 设备信息
    JzERHIType GetRHIType() const
    {
        return rhiType;
    }
    virtual String GetDeviceName() const    = 0;
    virtual String GetVendorName() const    = 0;
    virtual String GetDriverVersion() const = 0;

    // 资源创建接口
    virtual std::shared_ptr<JzRHIBuffer>      CreateBuffer(const BufferDesc &desc)            = 0;
    virtual std::shared_ptr<JzRHITexture>     CreateTexture(const TextureDesc &desc)          = 0;
    virtual std::shared_ptr<JzRHIShader>      CreateShader(const ShaderDesc &desc)            = 0;
    virtual std::shared_ptr<JzRHIPipeline>    CreatePipeline(const PipelineDesc &desc)        = 0;
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
    virtual void SetScissor(constJz ScissorRect &scissor)   = 0;

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

/**
 * RHI工厂类
 * 负责创建不同图形API的RHI设备
 */
class JzRHIFactory {
public:
    static std::unique_ptr<JzRHIDevice> CreateDevice(JzERHIType rhiType);
    static std::vector<JzERHIType>      GetSupportedRHITypes();
    static JzERHIType                   GetDefaultRHIType();
    static Bool                         IsRHITypeSupported(JzERHIType rhiType);
    static String                       GetRHITypeName(JzERHIType rhiType);

private:
    JzRHIFactory() = default;
};

/**
 * RHI上下文管理器
 * 管理RHI设备的生命周期和全局状态
 */
class JzRHIContext {
public:
    static JzRHIContext &GetInstance();

    // 设备管理
    Bool Initialize(JzERHIType rhiType = JzERHIType::Unknown);
    void Shutdown();
    Bool IsInitialized() const
    {
        return device != nullptr;
    }

    // 获取当前设备
    std::shared_ptr<JzRHIDevice> GetDevice() const
    {
        return device;
    }
    JzERHIType GetRHIType() const
    {
        return device ? device->GetRHIType() : JzERHIType::Unknown;
    }

    // 命令队列管理
    std::shared_ptr<JzRHICommandQueue> GetCommandQueue() const
    {
        return commandQueue;
    }

    // 多线程支持
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;

private:
    JzRHIContext()                                = default;
    ~JzRHIContext()                               = default;
    JzRHIContext(const JzRHIContext &)            = delete;
    JzRHIContext &operator=(const JzRHIContext &) = delete;

    std::shared_ptr<JzRHIDevice>       device;
    std::shared_ptr<JzRHICommandQueue> commandQueue;
};

/**
 * RHI便利宏定义
 */
#define JZ_RHI_DEVICE() JzRHIContext::GetInstance().GetDevice()
#define JZ_RHI_COMMAND_QUEUE() JzRHIContext::GetInstance().GetCommandQueue()
#define JZ_RHI_TYPE() JzRHIContext::GetInstance().GetRHIType()

/**
 * RHI能力查询接口
 */
struct JzRHICapabilities {
    // 纹理支持
    U32 maxTextureSize        = 0;
    U32 maxTexture3DSize      = 0;
    U32 maxTextureArrayLayers = 0;
    U32 maxCubeMapTextureSize = 0;

    // 渲染目标支持
    U32 maxColorAttachments = 0;
    U32 maxRenderTargetSize = 0;

    // 着色器支持
    U32 maxVertexAttributes      = 0;
    U32 maxUniformBufferBindings = 0;
    U32 maxTextureUnits          = 0;

    // 几何体支持
    U32 maxVertices = 0;
    U32 maxIndices  = 0;

    // 多重采样支持
    U32 maxSamples = 0;

    // 计算着色器支持
    Bool supportsComputeShaders         = false;
    U32  maxComputeWorkGroupSize[3]     = {0, 0, 0};
    U32  maxComputeWorkGroupInvocations = 0;

    // 几何着色器支持
    Bool supportsGeometryShaders = false;

    // 细分着色器支持
    Bool supportsTessellationShaders = false;

    // 多线程支持
    Bool supportsMultithreadedRendering = false;
    U32  maxRenderThreads               = 1;
};

/**
 * RHI统计信息
 */
struct JzRHIStats {
    // 绘制调用统计
    U32 drawCalls = 0;
    U32 triangles = 0;
    U32 vertices  = 0;

    // 资源统计
    U32 buffers   = 0;
    U32 textures  = 0;
    U32 shaders   = 0;
    U32 pipelines = 0;

    // 内存统计
    Size bufferMemory  = 0;
    Size textureMemory = 0;
    Size totalMemory   = 0;

    // 性能统计
    F32 frameTime = 0.0f;
    F32 gpuTime   = 0.0f;

    void Reset();
};

} // namespace JzRE