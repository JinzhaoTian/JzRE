/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <optional>
#include <set>
#include <sstream>
#include <vector>

#include <GLFW/glfw3.h>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanBuffer.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanFramebuffer.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanPipeline.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanShader.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanTexture.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanVertexArray.h"
#include "JzRE/Runtime/Platform/Window/JzIWindowBackend.h"

namespace JzRE {

namespace {

struct JzQueueFamilyIndices {
    std::optional<U32> graphics;
    std::optional<U32> present;

    Bool IsComplete() const
    {
        return graphics.has_value() && present.has_value();
    }
};

JzQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    JzQueueFamilyIndices indices;

    U32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (U32 i = 0; i < queueFamilyCount; ++i) {
        const auto &queueFamily = queueFamilies[i];

        if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            indices.graphics = i;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport == VK_TRUE) {
            indices.present = i;
        }

        if (indices.IsComplete()) {
            break;
        }
    }

    return indices;
}

Bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    U32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

    std::set<String> requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
    requiredExtensions.insert(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    for (const auto &extension : extensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    // VK_KHR_portability_subset is required only on implementations that expose it.
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
    requiredExtensions.erase(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    return requiredExtensions.empty();
}

Bool HasDeviceExtension(VkPhysicalDevice device, const char *name)
{
    U32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

    return std::any_of(
        extensions.begin(),
        extensions.end(),
        [name](const VkExtensionProperties &extension) {
            return std::strcmp(extension.extensionName, name) == 0;
        });
}

VkImageLayout ConvertResourceStateToImageLayout(JzERHIResourceState state)
{
    switch (state) {
        case JzERHIResourceState::Read:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case JzERHIResourceState::Write:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case JzERHIResourceState::ReadWrite:
            return VK_IMAGE_LAYOUT_GENERAL;
        case JzERHIResourceState::Unknown:
            break;
    }

    return VK_IMAGE_LAYOUT_GENERAL;
}

VkAccessFlags ConvertResourceStateToAccessFlags(JzERHIResourceState state)
{
    switch (state) {
        case JzERHIResourceState::Read:
            return VK_ACCESS_SHADER_READ_BIT;
        case JzERHIResourceState::Write:
            return VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case JzERHIResourceState::ReadWrite:
            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        case JzERHIResourceState::Unknown:
            break;
    }

    return 0;
}

VkImageAspectFlags GetImageAspectMask(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_D32_SFLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

} // namespace

JzVulkanDevice::JzVulkanDevice(JzIWindowBackend &windowBackend) :
    JzDevice(JzERHIType::Vulkan),
    m_windowBackend(&windowBackend)
{
    m_currentClear.clearColor   = true;
    m_currentClear.clearDepth   = true;
    m_currentClear.clearStencil = false;
    m_currentClear.colorR       = 0.08f;
    m_currentClear.colorG       = 0.08f;
    m_currentClear.colorB       = 0.09f;
    m_currentClear.colorA       = 1.0f;
    m_currentClear.depth        = 1.0f;

    if (!CreateInstance()) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to create Vulkan instance");
        return;
    }

    if (!CreateSurface()) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to create Vulkan surface");
        return;
    }

    if (!PickPhysicalDevice()) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to pick a suitable physical device");
        return;
    }

    if (!CreateLogicalDevice()) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to create logical device");
        return;
    }

    if (!CreateSwapchain() ||
        !CreateSwapchainImageViews() ||
        !CreateSwapchainRenderPass() ||
        !CreateSwapchainDepthResources() ||
        !CreateSwapchainFramebuffers()) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to create swapchain resources");
        return;
    }

    if (!CreateFrameSyncObjects()) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to create frame synchronization objects");
        return;
    }

    m_currentViewport = {
        0.0f,
        0.0f,
        static_cast<F32>(m_swapchainExtent.width),
        static_cast<F32>(m_swapchainExtent.height),
        0.0f,
        1.0f,
    };
    m_currentScissor = {
        0,
        0,
        m_swapchainExtent.width,
        m_swapchainExtent.height,
    };

    m_isInitialized = true;

    {
        const U32 whitePixel = 0xFFFFFFFFU;
        JzGPUTextureObjectDesc fallbackDesc{};
        fallbackDesc.type      = JzETextureResourceType::Texture2D;
        fallbackDesc.format    = JzETextureResourceFormat::RGBA8;
        fallbackDesc.width     = 1;
        fallbackDesc.height    = 1;
        fallbackDesc.data      = &whitePixel;
        fallbackDesc.debugName = "VulkanFallbackWhite";
        m_fallbackTexture      = std::dynamic_pointer_cast<JzVulkanTexture>(CreateTexture(fallbackDesc));
        if (!m_fallbackTexture) {
            JzRE_LOG_WARN("JzVulkanDevice: failed to create fallback texture");
        }
    }

    JzRE_LOG_INFO("JzVulkanDevice: initialized with '{}'", GetDeviceName());
}

JzVulkanDevice::~JzVulkanDevice()
{
    if (m_lifetimeFlag) {
        m_lifetimeFlag->store(false, std::memory_order_release);
    }

    Finish();

    m_boundTextures.clear();
    m_fallbackTexture.reset();

    DestroyFrameSyncObjects();
    DestroySwapchainObjects();

    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }

    if (m_surface != VK_NULL_HANDLE && m_instance != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }

    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

String JzVulkanDevice::GetDeviceName() const
{
    if (m_physicalDevice == VK_NULL_HANDLE) {
        return "Unknown Vulkan Device";
    }

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
    return properties.deviceName;
}

String JzVulkanDevice::GetVendorName() const
{
    if (m_physicalDevice == VK_NULL_HANDLE) {
        return "Unknown Vendor";
    }

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    switch (properties.vendorID) {
        case 0x10DE:
            return "NVIDIA";
        case 0x1002:
        case 0x1022:
            return "AMD";
        case 0x8086:
            return "Intel";
        case 0x106B:
            return "Apple";
        default: {
            std::ostringstream oss;
            oss << "Vendor(" << properties.vendorID << ")";
            return oss.str();
        }
    }
}

String JzVulkanDevice::GetDriverVersion() const
{
    if (m_physicalDevice == VK_NULL_HANDLE) {
        return "Unknown Version";
    }

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    std::ostringstream oss;
    oss << VK_VERSION_MAJOR(properties.driverVersion) << "."
        << VK_VERSION_MINOR(properties.driverVersion) << "."
        << VK_VERSION_PATCH(properties.driverVersion);
    return oss.str();
}

std::shared_ptr<JzGPUBufferObject> JzVulkanDevice::CreateBuffer(const JzGPUBufferObjectDesc &desc)
{
    auto buffer = std::make_shared<JzVulkanBuffer>(*this, desc);
    m_stats.buffers++;
    return buffer;
}

std::shared_ptr<JzGPUTextureObject> JzVulkanDevice::CreateTexture(const JzGPUTextureObjectDesc &desc)
{
    auto texture = std::make_shared<JzVulkanTexture>(*this, desc);
    m_stats.textures++;
    return texture;
}

std::shared_ptr<JzGPUShaderProgramObject> JzVulkanDevice::CreateShader(const JzShaderProgramDesc &desc)
{
    auto shader = std::make_shared<JzVulkanShader>(*this, desc);
    m_stats.shaders++;
    return shader;
}

std::shared_ptr<JzRHIPipeline> JzVulkanDevice::CreatePipeline(const JzPipelineDesc &desc)
{
    auto pipeline = std::make_shared<JzVulkanPipeline>(*this, desc);
    m_stats.pipelines++;
    return pipeline;
}

std::shared_ptr<JzGPUFramebufferObject> JzVulkanDevice::CreateFramebuffer(const String &debugName)
{
    return std::make_shared<JzVulkanFramebuffer>(debugName);
}

std::shared_ptr<JzGPUVertexArrayObject> JzVulkanDevice::CreateVertexArray(const String &debugName)
{
    return std::make_shared<JzVulkanVertexArray>(debugName);
}

std::shared_ptr<JzRHICommandList> JzVulkanDevice::CreateCommandList(const String &debugName)
{
    return std::make_shared<JzRHICommandList>(debugName);
}

void JzVulkanDevice::ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList)
{
    if (!commandList || commandList->IsEmpty()) {
        return;
    }

    const auto commands = commandList->GetCommands();
    for (const auto &command : commands) {
        DispatchCommand(command);
    }
}

void JzVulkanDevice::ExecuteCommandLists(const std::vector<std::shared_ptr<JzRHICommandList>> &commandLists)
{
    for (const auto &commandList : commandLists) {
        ExecuteCommandList(commandList);
    }
}

void JzVulkanDevice::DispatchCommand(const JzRHIRecordedCommand &command)
{
    switch (command.type) {
        case JzRHIECommandType::Clear: {
            if (const auto *payload = std::get_if<JzClearParams>(&command.payload)) {
                Clear(*payload);
            }
            break;
        }
        case JzRHIECommandType::Draw: {
            if (const auto *payload = std::get_if<JzDrawParams>(&command.payload)) {
                Draw(*payload);
            }
            break;
        }
        case JzRHIECommandType::DrawIndexed: {
            if (const auto *payload = std::get_if<JzDrawIndexedParams>(&command.payload)) {
                DrawIndexed(*payload);
            }
            break;
        }
        case JzRHIECommandType::BindPipeline: {
            if (const auto *payload = std::get_if<JzRHIBindPipelinePayload>(&command.payload)) {
                BindPipeline(payload->pipeline);
            }
            break;
        }
        case JzRHIECommandType::BindVertexArray: {
            if (const auto *payload = std::get_if<JzRHIBindVertexArrayPayload>(&command.payload)) {
                BindVertexArray(payload->vertexArray);
            }
            break;
        }
        case JzRHIECommandType::BindTexture: {
            if (const auto *payload = std::get_if<JzRHIBindTexturePayload>(&command.payload)) {
                BindTexture(payload->texture, payload->slot);
            }
            break;
        }
        case JzRHIECommandType::BindFramebuffer: {
            if (const auto *payload = std::get_if<JzRHIBindFramebufferPayload>(&command.payload)) {
                BindFramebuffer(payload->framebuffer);
            }
            break;
        }
        case JzRHIECommandType::SetViewport: {
            if (const auto *payload = std::get_if<JzViewport>(&command.payload)) {
                SetViewport(*payload);
            }
            break;
        }
        case JzRHIECommandType::SetScissor: {
            if (const auto *payload = std::get_if<JzScissorRect>(&command.payload)) {
                SetScissor(*payload);
            }
            break;
        }
        case JzRHIECommandType::BeginRenderPass: {
            if (const auto *payload = std::get_if<JzRHIBeginRenderPassPayload>(&command.payload)) {
                BeginRenderPass(*payload);
            }
            break;
        }
        case JzRHIECommandType::EndRenderPass: {
            if (const auto *payload = std::get_if<JzRHIEndRenderPassPayload>(&command.payload)) {
                EndRenderPass(*payload);
            }
            break;
        }
        case JzRHIECommandType::ResourceBarrier: {
            if (const auto *payload = std::get_if<JzRHIResourceBarrierPayload>(&command.payload)) {
                ResourceBarrier(payload->barriers);
            }
            break;
        }
        case JzRHIECommandType::BlitFramebufferToScreen: {
            if (const auto *payload = std::get_if<JzRHIBlitFramebufferToScreenPayload>(&command.payload)) {
                BlitFramebufferToScreen(
                    payload->framebuffer,
                    payload->srcWidth,
                    payload->srcHeight,
                    payload->dstWidth,
                    payload->dstHeight);
            }
            break;
        }
    }
}

void JzVulkanDevice::BeginRenderPass(const JzRHIBeginRenderPassPayload &payload)
{
    BindFramebuffer(payload.framebuffer);
    if (payload.renderPass) {
        payload.renderPass->OnBegin(*this, payload.framebuffer);
    }
}

void JzVulkanDevice::EndRenderPass(const JzRHIEndRenderPassPayload &payload)
{
    if (payload.renderPass) {
        payload.renderPass->OnEnd(*this);
    }
}

void JzVulkanDevice::BeginFrame()
{
    if (!m_isInitialized || m_device == VK_NULL_HANDLE || m_swapchain == VK_NULL_HANDLE) {
        return;
    }

    if (m_needsSwapchainRecreate) {
        if (!RecreateSwapchain()) {
            return;
        }
    }

    const auto framebufferSize = m_windowBackend->GetFramebufferSize();
    if (framebufferSize.x <= 0 || framebufferSize.y <= 0) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];

    vkWaitForFences(m_device, 1, &frame.inFlight, VK_TRUE, std::numeric_limits<U64>::max());

    const VkResult acquireResult = vkAcquireNextImageKHR(
        m_device,
        m_swapchain,
        std::numeric_limits<U64>::max(),
        frame.imageAvailable,
        VK_NULL_HANDLE,
        &m_currentImageIndex);

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
        m_needsSwapchainRecreate = true;
        return;
    }

    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        JzRE_LOG_ERROR("JzVulkanDevice: vkAcquireNextImageKHR failed ({})", static_cast<I32>(acquireResult));
        return;
    }

    if (acquireResult == VK_SUBOPTIMAL_KHR) {
        m_needsSwapchainRecreate = true;
    }

    if (!m_imagesInFlight.empty() && m_currentImageIndex < m_imagesInFlight.size()) {
        if (m_imagesInFlight[m_currentImageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(m_device, 1, &m_imagesInFlight[m_currentImageIndex], VK_TRUE, std::numeric_limits<U64>::max());
        }
        m_imagesInFlight[m_currentImageIndex] = frame.inFlight;
    }

    vkResetFences(m_device, 1, &frame.inFlight);
    vkResetCommandPool(m_device, frame.commandPool, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(frame.commandBuffer, &beginInfo) != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanDevice: vkBeginCommandBuffer failed");
        return;
    }

    if (!BeginSwapchainRenderPass()) {
        vkEndCommandBuffer(frame.commandBuffer);
        return;
    }

    m_isFrameActive   = true;
    m_readyForPresent = false;
    m_boundTextures.clear();

    m_stats.drawCalls = 0;
    m_stats.triangles = 0;
    m_stats.vertices  = 0;
}

void JzVulkanDevice::EndFrame()
{
    if (!m_isInitialized || !m_isFrameActive) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];

    EndSwapchainRenderPass();

    if (m_pendingBlitTexture &&
        m_pendingBlitTexture->GetImage() != VK_NULL_HANDLE &&
        m_currentImageIndex < m_swapchainImages.size()) {
        const VkImageLayout sourceOriginalLayout = m_pendingBlitTexture->GetLayout();

        VkImageMemoryBarrier srcToTransfer{};
        srcToTransfer.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        srcToTransfer.oldLayout           = sourceOriginalLayout;
        srcToTransfer.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcToTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcToTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcToTransfer.image               = m_pendingBlitTexture->GetImage();
        srcToTransfer.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        srcToTransfer.subresourceRange.baseMipLevel   = 0;
        srcToTransfer.subresourceRange.levelCount     = 1;
        srcToTransfer.subresourceRange.baseArrayLayer = 0;
        srcToTransfer.subresourceRange.layerCount     = 1;
        srcToTransfer.srcAccessMask                   = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        srcToTransfer.dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;

        VkImageMemoryBarrier dstToTransfer{};
        dstToTransfer.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        dstToTransfer.oldLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        dstToTransfer.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstToTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstToTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstToTransfer.image               = m_swapchainImages[m_currentImageIndex];
        dstToTransfer.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        dstToTransfer.subresourceRange.baseMipLevel   = 0;
        dstToTransfer.subresourceRange.levelCount     = 1;
        dstToTransfer.subresourceRange.baseArrayLayer = 0;
        dstToTransfer.subresourceRange.layerCount     = 1;
        dstToTransfer.srcAccessMask                   = 0;
        dstToTransfer.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

        std::array<VkImageMemoryBarrier, 2> barriersToTransfer = {
            srcToTransfer,
            dstToTransfer,
        };

        vkCmdPipelineBarrier(
            frame.commandBuffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            static_cast<U32>(barriersToTransfer.size()),
            barriersToTransfer.data());

        VkImageBlit blit{};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel   = 0;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.srcOffsets[0]             = {0, 0, 0};
        blit.srcOffsets[1]             = {
            static_cast<I32>(std::max<U32>(1U, m_pendingBlitSrcWidth)),
            static_cast<I32>(std::max<U32>(1U, m_pendingBlitSrcHeight)),
            1,
        };

        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel   = 0;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        blit.dstOffsets[0]             = {0, 0, 0};
        blit.dstOffsets[1]             = {
            static_cast<I32>(std::max<U32>(1U, m_pendingBlitDstWidth)),
            static_cast<I32>(std::max<U32>(1U, m_pendingBlitDstHeight)),
            1,
        };

        vkCmdBlitImage(
            frame.commandBuffer,
            m_pendingBlitTexture->GetImage(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            m_swapchainImages[m_currentImageIndex],
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blit,
            VK_FILTER_LINEAR);

        VkImageMemoryBarrier srcRestore = srcToTransfer;
        srcRestore.oldLayout            = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcRestore.newLayout            = (sourceOriginalLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                                              ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                                              : sourceOriginalLayout;
        srcRestore.srcAccessMask        = VK_ACCESS_TRANSFER_READ_BIT;
        srcRestore.dstAccessMask        = (srcRestore.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                                              ? VK_ACCESS_SHADER_READ_BIT
                                              : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkImageMemoryBarrier dstRestore = dstToTransfer;
        dstRestore.oldLayout            = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstRestore.newLayout            = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        dstRestore.srcAccessMask        = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstRestore.dstAccessMask        = 0;

        std::array<VkImageMemoryBarrier, 2> barriersToPresent = {
            srcRestore,
            dstRestore,
        };

        vkCmdPipelineBarrier(
            frame.commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            static_cast<U32>(barriersToPresent.size()),
            barriersToPresent.data());

        m_pendingBlitTexture->SetLayout(srcRestore.newLayout);
    }

    if (vkEndCommandBuffer(frame.commandBuffer) != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanDevice: vkEndCommandBuffer failed");
        return;
    }

    m_pendingBlitTexture.reset();
    m_pendingBlitSrcWidth  = 0;
    m_pendingBlitSrcHeight = 0;
    m_pendingBlitDstWidth  = 0;
    m_pendingBlitDstHeight = 0;

    m_isFrameActive   = false;
    m_readyForPresent = true;
}

void JzVulkanDevice::SetRenderState(const JzRenderState &state)
{
    m_currentRenderState = state;
}

void JzVulkanDevice::SetViewport(const JzViewport &viewport)
{
    m_currentViewport = viewport;

    if (!m_isFrameActive) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];

    VkViewport vkViewport{};
    vkViewport.x        = viewport.x;
    vkViewport.y        = viewport.y;
    vkViewport.width    = viewport.width;
    vkViewport.height   = viewport.height;
    vkViewport.minDepth = viewport.minDepth;
    vkViewport.maxDepth = viewport.maxDepth;

    vkCmdSetViewport(frame.commandBuffer, 0, 1, &vkViewport);
}

void JzVulkanDevice::SetScissor(const JzScissorRect &scissor)
{
    m_currentScissor = scissor;

    if (!m_isFrameActive) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];

    VkRect2D vkScissor{};
    vkScissor.offset = {scissor.x, scissor.y};
    vkScissor.extent = {scissor.width, scissor.height};

    vkCmdSetScissor(frame.commandBuffer, 0, 1, &vkScissor);
}

void JzVulkanDevice::Clear(const JzClearParams &params)
{
    m_currentClear = params;

    if (!m_isFrameActive) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];

    std::array<VkClearAttachment, 2> clearAttachments{};
    U32                               attachmentCount = 0;

    if (params.clearColor) {
        auto &colorAttachment          = clearAttachments[attachmentCount++];
        colorAttachment.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        colorAttachment.colorAttachment = 0;
        colorAttachment.clearValue.color = {
            {params.colorR, params.colorG, params.colorB, params.colorA},
        };
    }

    if (params.clearDepth || params.clearStencil) {
        auto &depthAttachment = clearAttachments[attachmentCount++];
        depthAttachment.aspectMask = 0;
        if (params.clearDepth) {
            depthAttachment.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (params.clearStencil) {
            depthAttachment.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        depthAttachment.clearValue.depthStencil = {
            params.depth,
            params.stencil,
        };
    }

    if (attachmentCount == 0) {
        return;
    }

    VkClearRect clearRect{};
    clearRect.baseArrayLayer = 0;
    clearRect.layerCount     = 1;
    clearRect.rect.offset    = {0, 0};
    clearRect.rect.extent    = m_swapchainExtent;

    vkCmdClearAttachments(
        frame.commandBuffer,
        attachmentCount,
        clearAttachments.data(),
        1,
        &clearRect);
}

void JzVulkanDevice::Draw(const JzDrawParams &params)
{
    if (!m_isFrameActive || params.vertexCount == 0) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];
    if (!m_currentPipeline || m_currentPipeline->GetPipeline() == VK_NULL_HANDLE) {
        return;
    }

    vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_currentPipeline->GetPipeline());
    m_currentPipeline->BindResources(frame.commandBuffer, m_boundTextures);

    if (m_currentVertexArray) {
        std::vector<std::pair<U32, VkBuffer>> bindings;
        bindings.reserve(m_currentVertexArray->GetVertexBuffers().size());

        for (const auto &entry : m_currentVertexArray->GetVertexBuffers()) {
            const auto vkBuffer = std::dynamic_pointer_cast<JzVulkanBuffer>(entry.second);
            if (!vkBuffer || vkBuffer->GetBuffer() == VK_NULL_HANDLE) {
                continue;
            }

            bindings.emplace_back(entry.first, vkBuffer->GetBuffer());
        }

        std::sort(bindings.begin(), bindings.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.first < rhs.first;
        });

        for (const auto &[binding, buffer] : bindings) {
            const VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(frame.commandBuffer, binding, 1, &buffer, &offset);
        }
    }

    vkCmdDraw(frame.commandBuffer,
              params.vertexCount,
              params.instanceCount,
              params.firstVertex,
              params.firstInstance);

    m_stats.drawCalls++;
    m_stats.vertices += params.vertexCount;
    if (params.primitiveType == JzEPrimitiveType::Triangles) {
        m_stats.triangles += params.vertexCount / 3;
    }
}

void JzVulkanDevice::DrawIndexed(const JzDrawIndexedParams &params)
{
    if (!m_isFrameActive || params.indexCount == 0) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];
    if (!m_currentPipeline || m_currentPipeline->GetPipeline() == VK_NULL_HANDLE) {
        return;
    }

    vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_currentPipeline->GetPipeline());
    m_currentPipeline->BindResources(frame.commandBuffer, m_boundTextures);

    if (m_currentVertexArray) {
        std::vector<std::pair<U32, VkBuffer>> bindings;
        bindings.reserve(m_currentVertexArray->GetVertexBuffers().size());

        for (const auto &entry : m_currentVertexArray->GetVertexBuffers()) {
            const auto vkBuffer = std::dynamic_pointer_cast<JzVulkanBuffer>(entry.second);
            if (!vkBuffer || vkBuffer->GetBuffer() == VK_NULL_HANDLE) {
                continue;
            }

            bindings.emplace_back(entry.first, vkBuffer->GetBuffer());
        }

        std::sort(bindings.begin(), bindings.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.first < rhs.first;
        });

        for (const auto &[binding, buffer] : bindings) {
            const VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(frame.commandBuffer, binding, 1, &buffer, &offset);
        }

        const auto indexBuffer = std::dynamic_pointer_cast<JzVulkanBuffer>(m_currentVertexArray->GetIndexBuffer());
        if (!indexBuffer || indexBuffer->GetBuffer() == VK_NULL_HANDLE) {
            return;
        }
        vkCmdBindIndexBuffer(frame.commandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    } else {
        return;
    }

    vkCmdDrawIndexed(frame.commandBuffer,
                     params.indexCount,
                     params.instanceCount,
                     params.firstIndex,
                     params.vertexOffset,
                     params.firstInstance);

    m_stats.drawCalls++;
    m_stats.vertices += params.indexCount;
    if (params.primitiveType == JzEPrimitiveType::Triangles) {
        m_stats.triangles += params.indexCount / 3;
    }
}

void JzVulkanDevice::BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline)
{
    m_currentPipeline = std::dynamic_pointer_cast<JzVulkanPipeline>(std::move(pipeline));
}

void JzVulkanDevice::BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray)
{
    m_currentVertexArray = std::dynamic_pointer_cast<JzVulkanVertexArray>(std::move(vertexArray));
}

void JzVulkanDevice::BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot)
{
    if (!texture) {
        m_boundTextures.erase(slot);
        return;
    }

    auto vkTexture = std::dynamic_pointer_cast<JzVulkanTexture>(std::move(texture));
    if (!vkTexture) {
        m_boundTextures.erase(slot);
        return;
    }

    m_boundTextures[slot] = std::move(vkTexture);
}

void JzVulkanDevice::BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer)
{
    m_currentFramebuffer = std::dynamic_pointer_cast<JzVulkanFramebuffer>(std::move(framebuffer));
}

void JzVulkanDevice::BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                             U32 srcWidth, U32 srcHeight,
                                             U32 dstWidth, U32 dstHeight)
{
    (void)framebuffer;
    (void)srcWidth;
    (void)srcHeight;
    (void)dstWidth;
    (void)dstHeight;
    // Runtime currently records render passes with framebuffer bind semantics that are
    // emulated on Vulkan by direct swapchain rendering. Keep blit as a no-op here so
    // command stream remains unified while avoiding an extra overwrite of swapchain.
}

void JzVulkanDevice::ResourceBarrier(const std::vector<JzRHIResourceBarrier> &barriers)
{
    if (!m_isFrameActive || barriers.empty()) {
        return;
    }

    auto &frame = m_frames[m_currentFrameIndex];

    std::vector<VkImageMemoryBarrier> imageBarriers;
    imageBarriers.reserve(barriers.size());

    for (const auto &barrier : barriers) {
        if (barrier.type != JzEResourceType::Texture || !barrier.resource) {
            continue;
        }

        auto texture = std::dynamic_pointer_cast<JzVulkanTexture>(barrier.resource);
        if (!texture || texture->GetImage() == VK_NULL_HANDLE) {
            continue;
        }

        const VkImageLayout oldLayout = texture->GetLayout();
        const VkImageLayout newLayout = ConvertResourceStateToImageLayout(barrier.after);

        if (oldLayout == newLayout) {
            continue;
        }

        VkImageMemoryBarrier imageBarrier{};
        imageBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.oldLayout           = oldLayout;
        imageBarrier.newLayout           = newLayout;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.image               = texture->GetImage();
        imageBarrier.subresourceRange.aspectMask     = GetImageAspectMask(texture->GetVkFormat());
        imageBarrier.subresourceRange.baseMipLevel   = 0;
        imageBarrier.subresourceRange.levelCount     = 1;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount     = 1;
        imageBarrier.srcAccessMask                   = ConvertResourceStateToAccessFlags(barrier.before);
        imageBarrier.dstAccessMask                   = ConvertResourceStateToAccessFlags(barrier.after);

        imageBarriers.push_back(imageBarrier);
        texture->SetLayout(newLayout);
    }

    if (imageBarriers.empty()) {
        return;
    }

    vkCmdPipelineBarrier(
        frame.commandBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        static_cast<U32>(imageBarriers.size()),
        imageBarriers.data());
}

void JzVulkanDevice::Flush()
{
    if (!m_isInitialized || !m_readyForPresent) {
        return;
    }

    const Bool submitOk = SubmitAndPresent();
    m_readyForPresent = false;

    if (submitOk) {
        m_currentFrameIndex = (m_currentFrameIndex + 1) % __MAX_FRAMES_IN_FLIGHT;
    }
}

void JzVulkanDevice::Finish()
{
    if (m_device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_device);
    }
}

Bool JzVulkanDevice::SupportsMultithreading() const
{
    return true;
}

void JzVulkanDevice::RequestSwapchainRecreate()
{
    m_needsSwapchainRecreate = true;
}

Bool JzVulkanDevice::ExecuteImmediate(const std::function<void(VkCommandBuffer)> &recordFn)
{
    if (!m_isInitialized || !recordFn) {
        return false;
    }

    VkCommandPool transientPool = VK_NULL_HANDLE;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &transientPool) != VK_SUCCESS) {
        return false;
    }

    VkCommandBuffer transientCommandBuffer = VK_NULL_HANDLE;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = transientPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(m_device, &allocInfo, &transientCommandBuffer) != VK_SUCCESS) {
        vkDestroyCommandPool(m_device, transientPool, nullptr);
        return false;
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(transientCommandBuffer, &beginInfo) != VK_SUCCESS) {
        vkDestroyCommandPool(m_device, transientPool, nullptr);
        return false;
    }

    recordFn(transientCommandBuffer);

    vkEndCommandBuffer(transientCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &transientCommandBuffer;

    const VkResult submitResult = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    if (submitResult == VK_SUCCESS) {
        vkQueueWaitIdle(m_graphicsQueue);
    }

    vkDestroyCommandPool(m_device, transientPool, nullptr);

    return submitResult == VK_SUCCESS;
}

U32 JzVulkanDevice::FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties) const
{
    if (m_physicalDevice == VK_NULL_HANDLE) {
        return 0;
    }

    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);

    for (U32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        const Bool matchesType = ((typeFilter & (1U << i)) != 0);
        const Bool matchesProperties =
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties;

        if (matchesType && matchesProperties) {
            return i;
        }
    }

    JzRE_LOG_WARN("JzVulkanDevice: failed to find suitable memory type, fallback to index 0");
    return 0;
}

VkCommandPool JzVulkanDevice::GetCurrentCommandPool() const
{
    return m_frames[m_currentFrameIndex].commandPool;
}

VkCommandBuffer JzVulkanDevice::GetCurrentCommandBuffer() const
{
    return m_frames[m_currentFrameIndex].commandBuffer;
}

Bool JzVulkanDevice::CreateInstance()
{
    U32                 glfwExtensionCount = 0;
    const char *const  *glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (!glfwExtensions || glfwExtensionCount == 0) {
        return false;
    }

    std::vector<const char *> enabledExtensions;
    enabledExtensions.reserve(glfwExtensionCount + 2);
    for (U32 i = 0; i < glfwExtensionCount; ++i) {
        enabledExtensions.push_back(glfwExtensions[i]);
    }

#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    enabledExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
#ifdef VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    enabledExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName   = "JzRE";
    applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    applicationInfo.pEngineName        = "JzRE";
    applicationInfo.engineVersion      = VK_MAKE_API_VERSION(0, 1, 0, 0);
    applicationInfo.apiVersion         = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo        = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount   = static_cast<U32>(enabledExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

#ifdef VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    const VkResult createResult = vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
    return createResult == VK_SUCCESS;
}

Bool JzVulkanDevice::CreateSurface()
{
    auto *glfwWindow = static_cast<GLFWwindow *>(m_windowBackend->GetPlatformWindowHandle());
    if (!glfwWindow) {
        return false;
    }

    const VkResult result = glfwCreateWindowSurface(m_instance, glfwWindow, nullptr, &m_surface);
    return result == VK_SUCCESS;
}

Bool JzVulkanDevice::PickPhysicalDevice()
{
    U32 deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto &device : devices) {
        const auto queueFamilies = FindQueueFamilies(device, m_surface);
        if (!queueFamilies.IsComplete()) {
            continue;
        }

        if (!CheckDeviceExtensionSupport(device)) {
            continue;
        }

        const auto swapchainSupport = QuerySwapchainSupport(device);
        if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
            continue;
        }

        m_physicalDevice            = device;
        m_graphicsQueueFamilyIndex  = queueFamilies.graphics.value();
        m_presentQueueFamilyIndex   = queueFamilies.present.value();
        return true;
    }

    return false;
}

Bool JzVulkanDevice::CreateLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<U32>                         uniqueQueueFamilies = {
        m_graphicsQueueFamilyIndex,
        m_presentQueueFamilyIndex,
    };

    const F32 queuePriority = 1.0f;
    for (const U32 queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
    if (HasDeviceExtension(m_physicalDevice, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
        deviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
    }
#endif

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount    = static_cast<U32>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures        = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount   = static_cast<U32>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    const VkResult createResult = vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
    if (createResult != VK_SUCCESS) {
        return false;
    }

    vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_presentQueueFamilyIndex, 0, &m_presentQueue);

    return true;
}

Bool JzVulkanDevice::CreateSwapchain()
{
    const auto support = QuerySwapchainSupport(m_physicalDevice);
    if (support.formats.empty() || support.presentModes.empty()) {
        return false;
    }

    const auto surfaceFormat = ChooseSwapSurfaceFormat(support.formats);
    const auto presentMode   = ChoosePresentMode(support.presentModes);
    const auto extent        = ChooseSwapExtent(support.capabilities);

    U32 imageCount = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
        imageCount = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = m_surface;
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    const U32 queueFamilyIndices[] = {
        m_graphicsQueueFamilyIndex,
        m_presentQueueFamilyIndex,
    };

    if (m_graphicsQueueFamilyIndex != m_presentQueueFamilyIndex) {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform   = support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    const VkResult createResult = vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain);
    if (createResult != VK_SUCCESS) {
        return false;
    }

    U32 swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, nullptr);

    m_swapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, m_swapchainImages.data());

    m_imagesInFlight.assign(m_swapchainImages.size(), VK_NULL_HANDLE);

    m_swapchainExtent      = extent;
    m_swapchainImageFormat = surfaceFormat.format;

    return true;
}

Bool JzVulkanDevice::CreateSwapchainImageViews()
{
    m_swapchainImageViews.resize(m_swapchainImages.size());

    for (Size i = 0; i < m_swapchainImages.size(); ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image    = m_swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format   = m_swapchainImageFormat;
        createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel   = 0;
        createInfo.subresourceRange.levelCount     = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS) {
            return false;
        }
    }

    return true;
}

Bool JzVulkanDevice::CreateSwapchainRenderPass()
{
    if (m_swapchainDepthFormat == VK_FORMAT_UNDEFINED) {
        m_swapchainDepthFormat = FindSupportedDepthFormat();
    }
    if (m_swapchainDepthFormat == VK_FORMAT_UNDEFINED) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to find supported depth format");
        return false;
    }

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = m_swapchainImageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format         = m_swapchainDepthFormat;
    depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass      = 0;
    dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    const std::array<VkAttachmentDescription, 2> attachments = {
        colorAttachment,
        depthAttachment,
    };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<U32>(attachments.size());
    renderPassInfo.pAttachments    = attachments.data();
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    return vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_swapchainRenderPass) == VK_SUCCESS;
}

Bool JzVulkanDevice::CreateSwapchainDepthResources()
{
    if (m_swapchainDepthFormat == VK_FORMAT_UNDEFINED) {
        m_swapchainDepthFormat = FindSupportedDepthFormat();
    }
    if (m_swapchainDepthFormat == VK_FORMAT_UNDEFINED) {
        JzRE_LOG_ERROR("JzVulkanDevice: failed to find supported depth format");
        return false;
    }

    JzRE_LOG_INFO("JzVulkanDevice: using swapchain depth format {}", static_cast<I32>(m_swapchainDepthFormat));

    m_swapchainDepthImages.resize(m_swapchainImages.size(), VK_NULL_HANDLE);
    m_swapchainDepthImageMemories.resize(m_swapchainImages.size(), VK_NULL_HANDLE);
    m_swapchainDepthImageViews.resize(m_swapchainImages.size(), VK_NULL_HANDLE);

    for (Size i = 0; i < m_swapchainImages.size(); ++i) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = m_swapchainExtent.width;
        imageInfo.extent.height = m_swapchainExtent.height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = m_swapchainDepthFormat;
        imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(m_device, &imageInfo, nullptr, &m_swapchainDepthImages[i]) != VK_SUCCESS) {
            JzRE_LOG_ERROR("JzVulkanDevice: vkCreateImage failed for swapchain depth image");
            return false;
        }

        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(m_device, m_swapchainDepthImages[i], &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_swapchainDepthImageMemories[i]) != VK_SUCCESS) {
            JzRE_LOG_ERROR("JzVulkanDevice: vkAllocateMemory failed for swapchain depth image");
            return false;
        }

        if (vkBindImageMemory(m_device, m_swapchainDepthImages[i], m_swapchainDepthImageMemories[i], 0) != VK_SUCCESS) {
            JzRE_LOG_ERROR("JzVulkanDevice: vkBindImageMemory failed for swapchain depth image");
            return false;
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image    = m_swapchainDepthImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format   = m_swapchainDepthFormat;
        viewInfo.subresourceRange.aspectMask     = GetImageAspectMask(m_swapchainDepthFormat);
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_swapchainDepthImageViews[i]) != VK_SUCCESS) {
            JzRE_LOG_ERROR("JzVulkanDevice: vkCreateImageView failed for swapchain depth image");
            return false;
        }
    }

    return true;
}

Bool JzVulkanDevice::CreateSwapchainFramebuffers()
{
    m_swapchainFramebuffers.resize(m_swapchainImageViews.size());

    for (Size i = 0; i < m_swapchainImageViews.size(); ++i) {
        if (i >= m_swapchainDepthImageViews.size() || m_swapchainDepthImageViews[i] == VK_NULL_HANDLE) {
            JzRE_LOG_ERROR("JzVulkanDevice: missing swapchain depth image view for framebuffer {}", i);
            return false;
        }

        const std::array<VkImageView, 2> attachments = {
            m_swapchainImageViews[i],
            m_swapchainDepthImageViews[i],
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = m_swapchainRenderPass;
        framebufferInfo.attachmentCount = static_cast<U32>(attachments.size());
        framebufferInfo.pAttachments    = attachments.data();
        framebufferInfo.width           = m_swapchainExtent.width;
        framebufferInfo.height          = m_swapchainExtent.height;
        framebufferInfo.layers          = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapchainFramebuffers[i]) != VK_SUCCESS) {
            return false;
        }
    }

    return true;
}

Bool JzVulkanDevice::CreateFrameSyncObjects()
{
    for (auto &frame : m_frames) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &frame.commandPool) != VK_SUCCESS) {
            return false;
        }

        VkCommandBufferAllocateInfo commandBufferAllocInfo{};
        commandBufferAllocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocInfo.commandPool        = frame.commandPool;
        commandBufferAllocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_device, &commandBufferAllocInfo, &frame.commandBuffer) != VK_SUCCESS) {
            return false;
        }

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &frame.imageAvailable) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &frame.renderFinished) != VK_SUCCESS ||
            vkCreateFence(m_device, &fenceInfo, nullptr, &frame.inFlight) != VK_SUCCESS) {
            return false;
        }
    }

    return true;
}

void JzVulkanDevice::DestroySwapchainObjects()
{
    for (auto framebuffer : m_swapchainFramebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }
    }
    m_swapchainFramebuffers.clear();

    for (auto imageView : m_swapchainDepthImageViews) {
        if (imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_device, imageView, nullptr);
        }
    }
    m_swapchainDepthImageViews.clear();

    for (Size i = 0; i < m_swapchainDepthImages.size(); ++i) {
        if (m_swapchainDepthImages[i] != VK_NULL_HANDLE) {
            vkDestroyImage(m_device, m_swapchainDepthImages[i], nullptr);
        }
        if (i < m_swapchainDepthImageMemories.size() && m_swapchainDepthImageMemories[i] != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_swapchainDepthImageMemories[i], nullptr);
        }
    }
    m_swapchainDepthImages.clear();
    m_swapchainDepthImageMemories.clear();

    if (m_swapchainRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_device, m_swapchainRenderPass, nullptr);
        m_swapchainRenderPass = VK_NULL_HANDLE;
    }

    for (auto imageView : m_swapchainImageViews) {
        if (imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_device, imageView, nullptr);
        }
    }
    m_swapchainImageViews.clear();

    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }

    m_swapchainImages.clear();
    m_imagesInFlight.clear();
}

void JzVulkanDevice::DestroyFrameSyncObjects()
{
    if (m_device == VK_NULL_HANDLE) {
        return;
    }

    for (auto &frame : m_frames) {
        if (frame.inFlight != VK_NULL_HANDLE) {
            vkDestroyFence(m_device, frame.inFlight, nullptr);
            frame.inFlight = VK_NULL_HANDLE;
        }
        if (frame.imageAvailable != VK_NULL_HANDLE) {
            vkDestroySemaphore(m_device, frame.imageAvailable, nullptr);
            frame.imageAvailable = VK_NULL_HANDLE;
        }
        if (frame.renderFinished != VK_NULL_HANDLE) {
            vkDestroySemaphore(m_device, frame.renderFinished, nullptr);
            frame.renderFinished = VK_NULL_HANDLE;
        }
        if (frame.commandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(m_device, frame.commandPool, nullptr);
            frame.commandPool = VK_NULL_HANDLE;
            frame.commandBuffer = VK_NULL_HANDLE;
        }
    }
}

Bool JzVulkanDevice::RecreateSwapchain()
{
    const auto framebufferSize = m_windowBackend->GetFramebufferSize();
    if (framebufferSize.x <= 0 || framebufferSize.y <= 0) {
        return false;
    }

    if (m_device == VK_NULL_HANDLE) {
        return false;
    }

    vkDeviceWaitIdle(m_device);

    DestroySwapchainObjects();

    if (!CreateSwapchain() ||
        !CreateSwapchainImageViews() ||
        !CreateSwapchainRenderPass() ||
        !CreateSwapchainDepthResources() ||
        !CreateSwapchainFramebuffers()) {
        return false;
    }

    m_currentViewport = {
        0.0f,
        0.0f,
        static_cast<F32>(m_swapchainExtent.width),
        static_cast<F32>(m_swapchainExtent.height),
        0.0f,
        1.0f,
    };
    m_currentScissor = {
        0,
        0,
        m_swapchainExtent.width,
        m_swapchainExtent.height,
    };

    m_needsSwapchainRecreate = false;
    return true;
}

JzVulkanDevice::JzVulkanSwapchainSupportDetails JzVulkanDevice::QuerySwapchainSupport(VkPhysicalDevice device) const
{
    JzVulkanSwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    U32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    U32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR JzVulkanDevice::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) const
{
    for (const auto &availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return formats.front();
}

VkPresentModeKHR JzVulkanDevice::ChoosePresentMode(const std::vector<VkPresentModeKHR> &presentModes) const
{
    for (const auto presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D JzVulkanDevice::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<U32>::max()) {
        return capabilities.currentExtent;
    }

    const auto framebufferSize = m_windowBackend->GetFramebufferSize();

    VkExtent2D actualExtent = {
        static_cast<U32>(std::max(1, framebufferSize.x)),
        static_cast<U32>(std::max(1, framebufferSize.y)),
    };

    actualExtent.width = std::clamp(
        actualExtent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height);

    return actualExtent;
}

VkFormat JzVulkanDevice::FindSupportedDepthFormat() const
{
    if (m_physicalDevice == VK_NULL_HANDLE) {
        return VK_FORMAT_UNDEFINED;
    }

    constexpr std::array<VkFormat, 3> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    for (const VkFormat format : candidates) {
        VkFormatProperties properties{};
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &properties);
        if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
            return format;
        }
    }

    return VK_FORMAT_UNDEFINED;
}

Bool JzVulkanDevice::BeginSwapchainRenderPass()
{
    auto &frame = m_frames[m_currentFrameIndex];

    if (m_currentImageIndex >= m_swapchainFramebuffers.size()) {
        return false;
    }

    VkClearValue clearValues[2]{};
    clearValues[0].color = {
        {m_currentClear.colorR, m_currentClear.colorG, m_currentClear.colorB, m_currentClear.colorA},
    };
    clearValues[1].depthStencil = {m_currentClear.depth, m_currentClear.stencil};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass        = m_swapchainRenderPass;
    renderPassBeginInfo.framebuffer       = m_swapchainFramebuffers[m_currentImageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = m_swapchainExtent;
    renderPassBeginInfo.clearValueCount   = 2;
    renderPassBeginInfo.pClearValues      = clearValues;

    vkCmdBeginRenderPass(frame.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x        = m_currentViewport.x;
    viewport.y        = m_currentViewport.y;
    viewport.width    = m_currentViewport.width;
    viewport.height   = m_currentViewport.height;
    viewport.minDepth = m_currentViewport.minDepth;
    viewport.maxDepth = m_currentViewport.maxDepth;

    VkRect2D scissor{};
    scissor.offset = {m_currentScissor.x, m_currentScissor.y};
    scissor.extent = {m_currentScissor.width, m_currentScissor.height};

    vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);

    return true;
}

void JzVulkanDevice::EndSwapchainRenderPass()
{
    auto &frame = m_frames[m_currentFrameIndex];
    vkCmdEndRenderPass(frame.commandBuffer);
}

Bool JzVulkanDevice::SubmitAndPresent()
{
    auto &frame = m_frames[m_currentFrameIndex];

    const VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &frame.imageAvailable;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &frame.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &frame.renderFinished;

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, frame.inFlight) != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanDevice: vkQueueSubmit failed");
        return false;
    }

    VkSwapchainKHR swapchains[] = {
        m_swapchain,
    };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &frame.renderFinished;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = swapchains;
    presentInfo.pImageIndices      = &m_currentImageIndex;

    const VkResult presentResult = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
        m_needsSwapchainRecreate = true;
        return true;
    }

    if (presentResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanDevice: vkQueuePresentKHR failed ({})", static_cast<I32>(presentResult));
        return false;
    }

    return true;
}

} // namespace JzRE
