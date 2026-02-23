/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzRHICapabilities.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIStats.h"

namespace JzRE {

class JzIWindowBackend;
class JzVulkanFramebuffer;
class JzVulkanPipeline;
class JzVulkanTexture;
class JzVulkanVertexArray;

/**
 * @brief Vulkan implementation of command-list based RHI device.
 */
class JzVulkanDevice : public JzDevice {
public:
    explicit JzVulkanDevice(JzIWindowBackend &windowBackend);
    ~JzVulkanDevice() override;

    Bool IsInitialized() const
    {
        return m_isInitialized;
    }

    String GetDeviceName() const override;
    String GetVendorName() const override;
    String GetDriverVersion() const override;

    std::shared_ptr<JzGPUBufferObject>        CreateBuffer(const JzGPUBufferObjectDesc &desc) override;
    std::shared_ptr<JzGPUTextureObject>       CreateTexture(const JzGPUTextureObjectDesc &desc) override;
    std::shared_ptr<JzGPUShaderProgramObject> CreateShader(const JzShaderProgramDesc &desc) override;
    std::shared_ptr<JzRHIPipeline>            CreatePipeline(const JzPipelineDesc &desc) override;
    std::shared_ptr<JzGPUFramebufferObject>   CreateFramebuffer(const String &debugName = "") override;
    std::shared_ptr<JzGPUVertexArrayObject>   CreateVertexArray(const String &debugName = "") override;
    std::shared_ptr<JzRHICommandList>         CreateCommandList(const String &debugName = "") override;

    void ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList) override;
    void ExecuteCommandLists(const std::vector<std::shared_ptr<JzRHICommandList>> &commandLists) override;

    void BeginFrame() override;
    void EndFrame() override;

    void Flush() override;
    void Finish() override;
    Bool SupportsMultithreading() const override;

    void RequestSwapchainRecreate();

    Bool ExecuteImmediate(const std::function<void(VkCommandBuffer)> &recordFn);

    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties) const;

    VkInstance GetVkInstance() const
    {
        return m_instance;
    }

    VkPhysicalDevice GetVkPhysicalDevice() const
    {
        return m_physicalDevice;
    }

    VkDevice GetVkDevice() const
    {
        return m_device;
    }

    VkQueue GetGraphicsQueue() const
    {
        return m_graphicsQueue;
    }

    VkQueue GetPresentQueue() const
    {
        return m_presentQueue;
    }

    U32 GetGraphicsQueueFamilyIndex() const
    {
        return m_graphicsQueueFamilyIndex;
    }

    U32 GetPresentQueueFamilyIndex() const
    {
        return m_presentQueueFamilyIndex;
    }

    VkExtent2D GetSwapchainExtent() const
    {
        return m_swapchainExtent;
    }

    VkFormat GetSwapchainFormat() const
    {
        return m_swapchainImageFormat;
    }

    VkRenderPass GetSwapchainRenderPass() const
    {
        return m_swapchainRenderPass;
    }

    VkCommandPool GetCurrentCommandPool() const;
    VkCommandBuffer GetCurrentCommandBuffer() const;

    const std::shared_ptr<std::atomic_bool> &GetLifetimeFlag() const
    {
        return m_lifetimeFlag;
    }

    Bool IsFrameRecording() const
    {
        return m_isFrameActive;
    }

    std::shared_ptr<JzVulkanTexture> GetFallbackTexture() const
    {
        return m_fallbackTexture;
    }

private:
    struct JzVulkanSwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR        capabilities{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };

    struct JzVulkanFrameSync {
        VkCommandPool   commandPool   = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkSemaphore     imageAvailable = VK_NULL_HANDLE;
        VkSemaphore     renderFinished = VK_NULL_HANDLE;
        VkFence         inFlight       = VK_NULL_HANDLE;
    };

    Bool CreateInstance();
    Bool CreateSurface();
    Bool PickPhysicalDevice();
    Bool CreateLogicalDevice();
    Bool CreateSwapchain();
    Bool CreateSwapchainImageViews();
    Bool CreateSwapchainRenderPass();
    Bool CreateSwapchainDepthResources();
    Bool CreateSwapchainFramebuffers();
    Bool CreateFrameSyncObjects();
    void DestroySwapchainObjects();
    void DestroyFrameSyncObjects();

    Bool RecreateSwapchain();

    JzVulkanSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device) const;

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) const;
    VkPresentModeKHR   ChoosePresentMode(const std::vector<VkPresentModeKHR> &presentModes) const;
    VkExtent2D         ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;
    VkFormat           FindSupportedDepthFormat() const;

    Bool BeginSwapchainRenderPass();
    void EndSwapchainRenderPass();
    Bool SubmitAndPresent();

    void DispatchCommand(const JzRHIRecordedCommand &command);

    void SetRenderState(const JzRenderState &state);
    void SetViewport(const JzViewport &viewport);
    void SetScissor(const JzScissorRect &scissor);
    void Clear(const JzClearParams &params);
    void Draw(const JzDrawParams &params);
    void DrawIndexed(const JzDrawIndexedParams &params);
    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline);
    void BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray);
    void BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot);
    void BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer);
    void BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                 U32 srcWidth, U32 srcHeight,
                                 U32 dstWidth, U32 dstHeight);
    void ResourceBarrier(const std::vector<JzRHIResourceBarrier> &barriers);

    void BeginRenderPass(const JzRHIBeginRenderPassPayload &payload);
    void EndRenderPass(const JzRHIEndRenderPassPayload &payload);

private:
    static constexpr U32 __MAX_FRAMES_IN_FLIGHT = 2;

    JzIWindowBackend *m_windowBackend = nullptr;

    Bool m_isInitialized         = false;
    Bool m_isFrameActive         = false;
    Bool m_readyForPresent       = false;
    Bool m_needsSwapchainRecreate = false;

    JzRHICapabilities m_capabilities;
    JzRHIStats        m_stats;

    JzRenderState m_currentRenderState;
    JzViewport    m_currentViewport;
    JzScissorRect m_currentScissor;
    JzClearParams m_currentClear;

    std::shared_ptr<JzVulkanPipeline>    m_currentPipeline;
    std::shared_ptr<JzVulkanVertexArray> m_currentVertexArray;
    std::shared_ptr<JzVulkanFramebuffer> m_currentFramebuffer;
    std::unordered_map<U32, std::shared_ptr<JzVulkanTexture>> m_boundTextures;

    VkInstance       m_instance       = VK_NULL_HANDLE;
    VkSurfaceKHR     m_surface        = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice         m_device         = VK_NULL_HANDLE;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue  = VK_NULL_HANDLE;

    U32 m_graphicsQueueFamilyIndex = 0;
    U32 m_presentQueueFamilyIndex  = 0;

    VkSwapchainKHR             m_swapchain           = VK_NULL_HANDLE;
    VkFormat                   m_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    VkFormat                   m_swapchainDepthFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D                 m_swapchainExtent{};
    std::vector<VkImage>       m_swapchainImages;
    std::vector<VkImageView>   m_swapchainImageViews;
    std::vector<VkImage>       m_swapchainDepthImages;
    std::vector<VkDeviceMemory> m_swapchainDepthImageMemories;
    std::vector<VkImageView>   m_swapchainDepthImageViews;
    VkRenderPass               m_swapchainRenderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_swapchainFramebuffers;

    std::array<JzVulkanFrameSync, __MAX_FRAMES_IN_FLIGHT> m_frames{};
    std::vector<VkFence>                                   m_imagesInFlight;

    U32 m_currentFrameIndex = 0;
    U32 m_currentImageIndex = 0;

    std::shared_ptr<JzVulkanTexture> m_pendingBlitTexture;
    std::shared_ptr<JzVulkanTexture> m_fallbackTexture;
    U32                              m_pendingBlitSrcWidth  = 0;
    U32                              m_pendingBlitSrcHeight = 0;
    U32                              m_pendingBlitDstWidth  = 0;
    U32                              m_pendingBlitDstHeight = 0;

    std::shared_ptr<std::atomic_bool> m_lifetimeFlag = std::make_shared<std::atomic_bool>(true);
};

} // namespace JzRE
