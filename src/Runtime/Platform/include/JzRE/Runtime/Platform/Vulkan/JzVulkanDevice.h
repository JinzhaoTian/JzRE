/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <array>
#include <functional>
#include <memory>
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
 * @brief Vulkan implementation of RHI device.
 *
 * This backend owns Vulkan instance/device/swapchain lifecycle and provides
 * the frame submission path used by runtime and editor.
 */
class JzVulkanDevice : public JzDevice {
public:
    /**
     * @brief Constructor.
     *
     * @param windowBackend Window backend used to create the Vulkan surface.
     */
    explicit JzVulkanDevice(JzIWindowBackend &windowBackend);

    /**
     * @brief Destructor.
     */
    ~JzVulkanDevice() override;

    /**
     * @brief Check whether Vulkan runtime objects were initialized.
     */
    Bool IsInitialized() const
    {
        return m_isInitialized;
    }

    String GetDeviceName() const override;
    String GetVendorName() const override;
    String GetDriverVersion() const override;

    std::shared_ptr<JzGPUBufferObject>      CreateBuffer(const JzGPUBufferObjectDesc &desc) override;
    std::shared_ptr<JzGPUTextureObject>     CreateTexture(const JzGPUTextureObjectDesc &desc) override;
    std::shared_ptr<JzGPUShaderProgramObject> CreateShader(const JzShaderProgramDesc &desc) override;
    std::shared_ptr<JzRHIPipeline>          CreatePipeline(const JzPipelineDesc &desc) override;
    std::shared_ptr<JzGPUFramebufferObject> CreateFramebuffer(const String &debugName = "") override;
    std::shared_ptr<JzGPUVertexArrayObject> CreateVertexArray(const String &debugName = "") override;
    std::shared_ptr<JzRHICommandList>       CreateCommandList(const String &debugName = "") override;

    void ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList) override;

    void BeginFrame() override;
    void EndFrame() override;

    void SetRenderState(const JzRenderState &state) override;
    void SetViewport(const JzViewport &viewport) override;
    void SetScissor(const JzScissorRect &scissor) override;
    void Clear(const JzClearParams &params) override;

    void Draw(const JzDrawParams &params) override;
    void DrawIndexed(const JzDrawIndexedParams &params) override;

    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline) override;
    void BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray) override;
    void BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot) override;
    void BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer) override;

    void BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                 U32 srcWidth, U32 srcHeight,
                                 U32 dstWidth, U32 dstHeight) override;

    void ResourceBarrier(const std::vector<JzRHIResourceBarrier> &barriers) override;

    /**
     * @brief Submit + present current frame.
     */
    void Flush() override;

    /**
     * @brief Wait until the device is idle.
     */
    void Finish() override;

    Bool SupportsMultithreading() const override;

    /**
     * @brief Request swapchain recreation on next frame boundary.
     */
    void RequestSwapchainRecreate();

    /**
     * @brief Execute an immediate command buffer and wait for completion.
     *
     * @param recordFn Command recording callback.
     * @return true if command execution succeeded.
     */
    Bool ExecuteImmediate(const std::function<void(VkCommandBuffer)> &recordFn);

    /**
     * @brief Find memory type index matching requirements.
     */
    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties) const;

    /**
     * @brief Vulkan instance used by this backend.
     */
    VkInstance GetVkInstance() const
    {
        return m_instance;
    }

    /**
     * @brief Vulkan physical device used by this backend.
     */
    VkPhysicalDevice GetVkPhysicalDevice() const
    {
        return m_physicalDevice;
    }

    /**
     * @brief Vulkan logical device used by this backend.
     */
    VkDevice GetVkDevice() const
    {
        return m_device;
    }

    /**
     * @brief Vulkan graphics queue handle.
     */
    VkQueue GetGraphicsQueue() const
    {
        return m_graphicsQueue;
    }

    /**
     * @brief Vulkan present queue handle.
     */
    VkQueue GetPresentQueue() const
    {
        return m_presentQueue;
    }

    /**
     * @brief Graphics queue family index.
     */
    U32 GetGraphicsQueueFamilyIndex() const
    {
        return m_graphicsQueueFamilyIndex;
    }

    /**
     * @brief Present queue family index.
     */
    U32 GetPresentQueueFamilyIndex() const
    {
        return m_presentQueueFamilyIndex;
    }

    /**
     * @brief Current swapchain extent.
     */
    VkExtent2D GetSwapchainExtent() const
    {
        return m_swapchainExtent;
    }

    /**
     * @brief Swapchain color format.
     */
    VkFormat GetSwapchainFormat() const
    {
        return m_swapchainImageFormat;
    }

    /**
     * @brief Render pass used for swapchain rendering.
     */
    VkRenderPass GetSwapchainRenderPass() const
    {
        return m_swapchainRenderPass;
    }

    /**
     * @brief Command pool for the current in-flight frame.
     */
    VkCommandPool GetCurrentCommandPool() const;

    /**
     * @brief Primary command buffer for the current in-flight frame.
     */
    VkCommandBuffer GetCurrentCommandBuffer() const;

    /**
     * @brief Returns true when command buffer recording is active for the frame.
     */
    Bool IsFrameRecording() const
    {
        return m_isFrameActive;
    }

private:
    struct JzVulkanSwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR        capabilities{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };

    struct JzVulkanFrameSync {
        VkCommandPool   commandPool           = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer         = VK_NULL_HANDLE;
        VkSemaphore     imageAvailable        = VK_NULL_HANDLE;
        VkSemaphore     renderFinished        = VK_NULL_HANDLE;
        VkFence         inFlight              = VK_NULL_HANDLE;
    };

    Bool CreateInstance();
    Bool CreateSurface();
    Bool PickPhysicalDevice();
    Bool CreateLogicalDevice();
    Bool CreateSwapchain();
    Bool CreateSwapchainImageViews();
    Bool CreateSwapchainRenderPass();
    Bool CreateSwapchainFramebuffers();
    Bool CreateFrameSyncObjects();
    void DestroySwapchainObjects();
    void DestroyFrameSyncObjects();

    Bool RecreateSwapchain();

    JzVulkanSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device) const;

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) const;
    VkPresentModeKHR   ChoosePresentMode(const std::vector<VkPresentModeKHR> &presentModes) const;
    VkExtent2D         ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

    Bool BeginSwapchainRenderPass();
    void EndSwapchainRenderPass();
    Bool SubmitAndPresent();

private:
    static constexpr U32 __MAX_FRAMES_IN_FLIGHT = 2;

    JzIWindowBackend *m_windowBackend = nullptr;

    Bool m_isInitialized      = false;
    Bool m_isFrameActive      = false;
    Bool m_readyForPresent    = false;
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

    VkInstance       m_instance       = VK_NULL_HANDLE;
    VkSurfaceKHR     m_surface        = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice         m_device         = VK_NULL_HANDLE;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue  = VK_NULL_HANDLE;

    U32 m_graphicsQueueFamilyIndex = 0;
    U32 m_presentQueueFamilyIndex  = 0;

    VkSwapchainKHR                m_swapchain          = VK_NULL_HANDLE;
    VkFormat                      m_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    VkExtent2D                    m_swapchainExtent{};
    std::vector<VkImage>          m_swapchainImages;
    std::vector<VkImageView>      m_swapchainImageViews;
    VkRenderPass                  m_swapchainRenderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer>    m_swapchainFramebuffers;

    std::array<JzVulkanFrameSync, __MAX_FRAMES_IN_FLIGHT> m_frames{};
    std::vector<VkFence>                                   m_imagesInFlight;

    U32 m_currentFrameIndex = 0;
    U32 m_currentImageIndex = 0;

    std::shared_ptr<JzVulkanTexture> m_pendingBlitTexture;
    U32                              m_pendingBlitSrcWidth  = 0;
    U32                              m_pendingBlitSrcHeight = 0;
    U32                              m_pendingBlitDstWidth  = 0;
    U32                              m_pendingBlitDstHeight = 0;
};

} // namespace JzRE
