/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRHIETypes.h"
#include "JzRHIDevice.h"
#include "JzRHICapabilities.h"
#include "JzRHICommandList.h"
#include "JzRHIFramebuffer.h"
#include "JzRHIShader.h"
#include "JzRHIStats.h"

namespace JzRE {
/**
 * @brief Vulkan Device Implementation (Stub)
 */
class JzVulkanDevice : public JzRHIDevice {
public:
    JzVulkanDevice();
    ~JzVulkanDevice() override;

    // 设备信息
    String GetDeviceName() const override;
    String GetVendorName() const override;
    String GetDriverVersion() const override;

    // 资源创建接口
    std::shared_ptr<JzRHIBuffer>      CreateBuffer(const JzBufferDesc &desc) override;
    std::shared_ptr<JzRHITexture>     CreateTexture(const JzTextureDesc &desc) override;
    std::shared_ptr<JzRHIShader>      CreateShader(const JzShaderDesc &desc) override;
    std::shared_ptr<JzRHIPipeline>    CreatePipeline(const JzPipelineDesc &desc) override;
    std::shared_ptr<JzRHIFramebuffer> CreateFramebuffer(const String &debugName = "") override;
    std::shared_ptr<JzRHIVertexArray> CreateVertexArray(const String &debugName = "") override;

    // 命令系统
    std::shared_ptr<JzRHICommandList> CreateCommandList(const String &debugName = "") override;
    void                              ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList) override;

    // 立即渲染模式
    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    // 渲染状态管理
    void SetRenderState(const JzRenderState &state) override;
    void SetViewport(const JzViewport &viewport) override;
    void SetScissor(const JzScissorRect &scissor) override;

    // 绘制接口
    void Clear(const JzClearParams &params) override;
    void Draw(const JzDrawParams &params) override;
    void DrawIndexed(const JzDrawIndexedParams &params) override;

    // 资源绑定
    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline) override;
    void BindVertexArray(std::shared_ptr<JzRHIVertexArray> vertexArray) override;
    void BindTexture(std::shared_ptr<JzRHITexture> texture, U32 slot) override;
    void BindFramebuffer(std::shared_ptr<JzRHIFramebuffer> framebuffer) override;

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

    JzRHICapabilities capabilities;
    JzRHIStats        stats;
};

} // namespace JzRE