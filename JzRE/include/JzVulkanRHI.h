#pragma once

#include "RHI.h"

namespace JzRE {

/**
 * Vulkan缓冲区实现（存根）
 * 未来可扩展为完整的Vulkan实现
 */
class VulkanBuffer : public RHIBuffer {
public:
    VulkanBuffer(const BufferDesc& desc);
    ~VulkanBuffer() override;

    void UpdateData(const void* data, Size size, Size offset = 0) override;
    void* MapBuffer() override;
    void UnmapBuffer() override;

private:
    // VkBuffer buffer;
    // VkDeviceMemory memory;
    // VkDevice device;
};

/**
 * Vulkan纹理实现（存根）
 */
class VulkanTexture : public RHITexture {
public:
    VulkanTexture(const TextureDesc& desc);
    ~VulkanTexture() override;

    void UpdateData(const void* data, U32 mipLevel = 0, U32 arrayIndex = 0) override;
    void GenerateMipmaps() override;

private:
    // VkImage image;
    // VkImageView imageView;
    // VkDeviceMemory memory;
    // VkSampler sampler;
};

/**
 * Vulkan着色器实现（存根）
 */
class VulkanShader : public RHIShader {
public:
    VulkanShader(const ShaderDesc& desc);
    ~VulkanShader() override;

private:
    // VkShaderModule shaderModule;
    // VkPipelineShaderStageCreateInfo stageInfo;
};

/**
 * Vulkan渲染管线实现（存根）
 */
class VulkanPipeline : public RHIPipeline {
public:
    VulkanPipeline(const PipelineDesc& desc);
    ~VulkanPipeline() override;

private:
    // VkPipeline pipeline;
    // VkPipelineLayout pipelineLayout;
    // VkRenderPass renderPass;
};

/**
 * Vulkan帧缓冲实现（存根）
 */
class VulkanFramebuffer : public RHIFramebuffer {
public:
    VulkanFramebuffer(const String& debugName = "");
    ~VulkanFramebuffer() override;

    void AttachColorTexture(std::shared_ptr<RHITexture> texture, U32 attachmentIndex = 0) override;
    void AttachDepthTexture(std::shared_ptr<RHITexture> texture) override;
    void AttachDepthStencilTexture(std::shared_ptr<RHITexture> texture) override;
    Bool IsComplete() const override;

private:
    // VkFramebuffer framebuffer;
    // VkRenderPass renderPass;
};

/**
 * Vulkan顶点数组对象实现（存根）
 */
class VulkanVertexArray : public RHIVertexArray {
public:
    VulkanVertexArray(const String& debugName = "");
    ~VulkanVertexArray() override;

    void BindVertexBuffer(std::shared_ptr<RHIBuffer> buffer, U32 binding = 0) override;
    void BindIndexBuffer(std::shared_ptr<RHIBuffer> buffer) override;
    void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) override;

private:
    // std::vector<VkBuffer> vertexBuffers;
    // VkBuffer indexBuffer;
};

/**
 * Vulkan设备实现（存根）
 */
class VulkanDevice : public RHIDevice {
public:
    VulkanDevice();
    ~VulkanDevice() override;

    // 设备信息
    String GetDeviceName() const override;
    String GetVendorName() const override;
    String GetDriverVersion() const override;

    // 资源创建接口
    std::shared_ptr<RHIBuffer> CreateBuffer(const BufferDesc& desc) override;
    std::shared_ptr<RHITexture> CreateTexture(const TextureDesc& desc) override;
    std::shared_ptr<RHIShader> CreateShader(const ShaderDesc& desc) override;
    std::shared_ptr<RHIPipeline> CreatePipeline(const PipelineDesc& desc) override;
    std::shared_ptr<RHIFramebuffer> CreateFramebuffer(const String& debugName = "") override;
    std::shared_ptr<RHIVertexArray> CreateVertexArray(const String& debugName = "") override;

    // 命令系统
    std::shared_ptr<RHICommandBuffer> CreateCommandBuffer(const String& debugName = "") override;
    void ExecuteCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer) override;

    // 立即渲染模式
    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    // 渲染状态管理
    void SetRenderState(const RenderState& state) override;
    void SetViewport(const Viewport& viewport) override;
    void SetScissor(const ScissorRect& scissor) override;

    // 绘制接口
    void Clear(const ClearParams& params) override;
    void Draw(const DrawParams& params) override;
    void DrawIndexed(const DrawIndexedParams& params) override;

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
    RHIStats stats;
};

} // namespace JzRE 