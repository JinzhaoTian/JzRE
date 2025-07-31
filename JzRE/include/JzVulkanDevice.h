#pragma once

#include "JzRHIDevice.h"

namespace JzRE {

/**
 * Vulkan设备实现（存根）
 */
class JzVulkanDevice : public JzRHIDevice {
public:
    VulkanDevice();
    ~VulkanDevice() override;

    // 设备信息
    String GetDeviceName() const override;
    String GetVendorName() const override;
    String GetDriverVersion() const override;

    // 资源创建接口
    std::shared_ptr<RHIBuffer>      CreateBuffer(const BufferDesc &desc) override;
    std::shared_ptr<RHITexture>     CreateTexture(const TextureDesc &desc) override;
    std::shared_ptr<RHIShader>      CreateShader(const ShaderDesc &desc) override;
    std::shared_ptr<RHIPipeline>    CreatePipeline(const PipelineDesc &desc) override;
    std::shared_ptr<RHIFramebuffer> CreateFramebuffer(const String &debugName = "") override;
    std::shared_ptr<RHIVertexArray> CreateVertexArray(const String &debugName = "") override;

    // 命令系统
    std::shared_ptr<RHICommandBuffer> CreateCommandBuffer(const String &debugName = "") override;
    void                              ExecuteCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer) override;

    // 立即渲染模式
    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    // 渲染状态管理
    void SetRenderState(const RenderState &state) override;
    void SetViewport(const Viewport &viewport) override;
    void SetScissor(const ScissorRect &scissor) override;

    // 绘制接口
    void Clear(const ClearParams &params) override;
    void Draw(const DrawParams &params) override;
    void DrawIndexed(const DrawIndexedParams &params) override;

    // 资源绑定
    void BindPipeline(std::shared_ptr<RHIPipeline> pipeline) override;
    void BindVertexArray(std::shared_ptr<RHIVertexArray> vertexArray) override;
    void BindTexture(std::shared_ptr<RHITexture> texture, U32 slot) override;
    void BindFramebuffer(std::shared_ptr<RHIFramebuffer> framebuffer) override;

    // 同步和等待
    void Flush() override;
    void Finish() override;

    // 多线程支持
    Bool SupportsMultithreading() const override;
    void MakeContextCurrent(U32 threadIndex = 0) override;

private:
    // VkInstance instance;
    // VkPhysicalDevice physicalDevice;
    // VkDevice device;
    // VkQueue graphicsQueue;
    // VkQueue presentQueue;
    // VkCommandPool commandPool;

    RHICapabilities capabilities;
    RHIStats        stats;
};

} // namespace JzRE