/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzRHICapabilities.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIStats.h"

#if defined(_WIN32)
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#endif

namespace JzRE {

class JzIWindowBackend;

#if defined(_WIN32)

class JzD3D12Buffer;
class JzD3D12Texture;
class JzD3D12Shader;
class JzD3D12Pipeline;
class JzD3D12VertexArray;
class JzD3D12Framebuffer;

/**
 * @brief Direct3D 12 implementation of the RHI device.
 */
class JzD3D12Device final : public JzDevice {
public:
    explicit JzD3D12Device(JzIWindowBackend &windowBackend);
    ~JzD3D12Device() override;

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

    ID3D12Device              *GetDevice() const;
    ID3D12GraphicsCommandList *GetCommandList() const;
    IDXGISwapChain3           *GetSwapChain() const;

    std::shared_ptr<JzD3D12Texture> GetFallbackTexture() const;

    Bool ExecuteImmediate(const std::function<void(ID3D12GraphicsCommandList *)> &recordFn);

    Bool IsFrameRecording() const;

private:
    struct FrameResources {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
        UINT64                                         fenceValue = 0;
    };

    void InitializeCapabilities();
    Bool CreateDevice();
    Bool CreateCommandObjects();
    Bool CreateSwapchain();
    Bool CreateRenderTargets();
    Bool CreateDepthBuffer();
    void ReleaseSwapchainResources();
    Bool ResizeSwapchain(U32 width, U32 height);
    void WaitForFrame(UINT frameIndex);
    void DispatchCommand(const JzRHIRecordedCommand &command);

    void BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline);
    void BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray);
    void BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot);
    void BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer);
    void SetViewport(const JzViewport &viewport);
    void SetScissor(const JzScissorRect &scissor);
    void Clear(const JzClearParams &params);
    void Draw(const JzDrawParams &params);
    void DrawIndexed(const JzDrawIndexedParams &params);
    void ResourceBarrier(const std::vector<JzRHIResourceBarrier> &barriers);
    void BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                 U32 srcWidth, U32 srcHeight,
                                 U32 dstWidth, U32 dstHeight);

private:
    static constexpr UINT __FRAME_COUNT = 2;

    JzIWindowBackend *m_windowBackend   = nullptr;
    Bool              m_isInitialized   = false;
    Bool              m_isFrameActive   = false;
    Bool              m_readyForPresent = false;

    JzRHICapabilities m_capabilities;
    JzRHIStats        m_stats;

    Microsoft::WRL::ComPtr<IDXGIFactory6>             m_factory;
    Microsoft::WRL::ComPtr<ID3D12Device>              m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>        m_commandQueue;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<IDXGISwapChain3>           m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>      m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>      m_dsvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource>            m_backBuffers[__FRAME_COUNT];
    Microsoft::WRL::ComPtr<ID3D12Resource>            m_depthBuffer;
    Microsoft::WRL::ComPtr<ID3D12Fence>               m_fence;
    HANDLE                                            m_fenceEvent = nullptr;

    FrameResources m_frames[__FRAME_COUNT];
    UINT           m_frameIndex = 0;
    UINT64         m_fenceValue = 0;

    UINT m_rtvDescriptorSize = 0;
    UINT m_dsvDescriptorSize = 0;

    JzViewport    m_currentViewport{};
    JzScissorRect m_currentScissor{};
    JzClearParams m_currentClear{};

    std::shared_ptr<JzD3D12Pipeline>    m_currentPipeline;
    std::shared_ptr<JzD3D12VertexArray> m_currentVertexArray;
    std::shared_ptr<JzD3D12Framebuffer> m_currentFramebuffer;

    std::unordered_map<U32, std::shared_ptr<JzD3D12Texture>> m_boundTextures;
    std::shared_ptr<JzD3D12Texture>                          m_fallbackTexture;
};

#endif // _WIN32

} // namespace JzRE
