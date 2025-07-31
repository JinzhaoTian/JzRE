#pragma once

#include "RHITypes.h"
#include "RHIResource.h"
#include "RHICommand.h"

namespace JzRE {

/**
 * RHI设备接口
 * 提供跨平台的图形设备抽象
 */
class RHIDevice {
public:
    RHIDevice(ERHIType rhiType) : rhiType(rhiType) {}
    virtual ~RHIDevice() = default;

    // 设备信息
    ERHIType GetRHIType() const { return rhiType; }
    virtual String GetDeviceName() const = 0;
    virtual String GetVendorName() const = 0;
    virtual String GetDriverVersion() const = 0;

    // 资源创建接口
    virtual std::shared_ptr<RHIBuffer> CreateBuffer(const BufferDesc& desc) = 0;
    virtual std::shared_ptr<RHITexture> CreateTexture(const TextureDesc& desc) = 0;
    virtual std::shared_ptr<RHIShader> CreateShader(const ShaderDesc& desc) = 0;
    virtual std::shared_ptr<RHIPipeline> CreatePipeline(const PipelineDesc& desc) = 0;
    virtual std::shared_ptr<RHIFramebuffer> CreateFramebuffer(const String& debugName = "") = 0;
    virtual std::shared_ptr<RHIVertexArray> CreateVertexArray(const String& debugName = "") = 0;

    // 命令系统
    virtual std::shared_ptr<RHICommandBuffer> CreateCommandBuffer(const String& debugName = "") = 0;
    virtual void ExecuteCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer) = 0;

    // 立即渲染模式（与命令缓冲模式互斥）
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;

    // 渲染状态管理
    virtual void SetRenderState(const RenderState& state) = 0;
    virtual void SetViewport(const Viewport& viewport) = 0;
    virtual void SetScissor(const ScissorRect& scissor) = 0;

    // 绘制接口
    virtual void Clear(const ClearParams& params) = 0;
    virtual void Draw(const DrawParams& params) = 0;
    virtual void DrawIndexed(const DrawIndexedParams& params) = 0;

    // 资源绑定
    virtual void BindPipeline(std::shared_ptr<RHIPipeline> pipeline) = 0;
    virtual void BindVertexArray(std::shared_ptr<RHIVertexArray> vertexArray) = 0;
    virtual void BindTexture(std::shared_ptr<RHITexture> texture, U32 slot) = 0;
    virtual void BindFramebuffer(std::shared_ptr<RHIFramebuffer> framebuffer) = 0;

    // 同步和等待
    virtual void Flush() = 0;
    virtual void Finish() = 0;

    // 多线程支持
    virtual Bool SupportsMultithreading() const = 0;
    virtual void MakeContextCurrent(U32 threadIndex = 0) = 0;

protected:
    ERHIType rhiType;
};

/**
 * RHI工厂类
 * 负责创建不同图形API的RHI设备
 */
class RHIFactory {
public:
    static std::unique_ptr<RHIDevice> CreateDevice(ERHIType rhiType);
    static std::vector<ERHIType> GetSupportedRHITypes();
    static ERHIType GetDefaultRHIType();
    static Bool IsRHITypeSupported(ERHIType rhiType);
    static String GetRHITypeName(ERHIType rhiType);

private:
    RHIFactory() = default;
};

/**
 * RHI上下文管理器
 * 管理RHI设备的生命周期和全局状态
 */
class RHIContext {
public:
    static RHIContext& GetInstance();

    // 设备管理
    Bool Initialize(ERHIType rhiType = ERHIType::Unknown);
    void Shutdown();
    Bool IsInitialized() const { return device != nullptr; }

    // 获取当前设备
    std::shared_ptr<RHIDevice> GetDevice() const { return device; }
    ERHIType GetRHIType() const { return device ? device->GetRHIType() : ERHIType::Unknown; }

    // 命令队列管理
    std::shared_ptr<RHICommandQueue> GetCommandQueue() const { return commandQueue; }

    // 多线程支持
    void SetThreadCount(U32 threadCount);
    U32 GetThreadCount() const;

private:
    RHIContext() = default;
    ~RHIContext() = default;
    RHIContext(const RHIContext&) = delete;
    RHIContext& operator=(const RHIContext&) = delete;

    std::shared_ptr<RHIDevice> device;
    std::shared_ptr<RHICommandQueue> commandQueue;
};

/**
 * RHI便利宏定义
 */
#define RHI_DEVICE() RHIContext::GetInstance().GetDevice()
#define RHI_COMMAND_QUEUE() RHIContext::GetInstance().GetCommandQueue()
#define RHI_TYPE() RHIContext::GetInstance().GetRHIType()

/**
 * RHI能力查询接口
 */
struct RHICapabilities {
    // 纹理支持
    U32 maxTextureSize = 0;
    U32 maxTexture3DSize = 0;
    U32 maxTextureArrayLayers = 0;
    U32 maxCubeMapTextureSize = 0;

    // 渲染目标支持
    U32 maxColorAttachments = 0;
    U32 maxRenderTargetSize = 0;

    // 着色器支持
    U32 maxVertexAttributes = 0;
    U32 maxUniformBufferBindings = 0;
    U32 maxTextureUnits = 0;

    // 几何体支持
    U32 maxVertices = 0;
    U32 maxIndices = 0;

    // 多重采样支持
    U32 maxSamples = 0;

    // 计算着色器支持
    Bool supportsComputeShaders = false;
    U32 maxComputeWorkGroupSize[3] = {0, 0, 0};
    U32 maxComputeWorkGroupInvocations = 0;

    // 几何着色器支持
    Bool supportsGeometryShaders = false;

    // 细分着色器支持
    Bool supportsTessellationShaders = false;

    // 多线程支持
    Bool supportsMultithreadedRendering = false;
    U32 maxRenderThreads = 1;
};

/**
 * RHI统计信息
 */
struct RHIStats {
    // 绘制调用统计
    U32 drawCalls = 0;
    U32 triangles = 0;
    U32 vertices = 0;

    // 资源统计
    U32 buffers = 0;
    U32 textures = 0;
    U32 shaders = 0;
    U32 pipelines = 0;

    // 内存统计
    Size bufferMemory = 0;
    Size textureMemory = 0;
    Size totalMemory = 0;

    // 性能统计
    F32 frameTime = 0.0f;
    F32 gpuTime = 0.0f;
    
    void Reset();
};

} // namespace JzRE 