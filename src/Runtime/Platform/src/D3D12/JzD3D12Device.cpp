
/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/D3D12/JzD3D12Device.h"

#if defined(_WIN32)

#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Buffer.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Framebuffer.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Pipeline.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Shader.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Texture.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12VertexArray.h"
#include "JzRE/Runtime/Platform/Window/JzIWindowBackend.h"

namespace JzRE {

namespace {

/**
 * @brief Convert a wide string to UTF-8.
 *
 * @param text Wide string to convert.
 *
 * @return Converted UTF-8 string, or empty string on failure.
 */
String WideToUtf8(const WString &text)
{
    if (text.empty()) {
        return {};
    }

    const int required = WideCharToMultiByte(CP_UTF8,
                                             0,
                                             text.c_str(),
                                             static_cast<int>(text.size()),
                                             nullptr,
                                             0,
                                             nullptr,
                                             nullptr);
    if (required <= 0) {
        return {};
    }

    String    result(static_cast<size_t>(required), '\0');
    const int converted = WideCharToMultiByte(CP_UTF8,
                                              0,
                                              text.c_str(),
                                              static_cast<int>(text.size()),
                                              result.data(),
                                              required,
                                              nullptr,
                                              nullptr);
    if (converted != required) {
        return {};
    }

    return result;
}

D3D12_RESOURCE_STATES ConvertResourceState(JzERHIResourceState state, Bool isDepth)
{
    switch (state) {
        case JzERHIResourceState::Read:
            return isDepth ? D3D12_RESOURCE_STATE_DEPTH_READ : (D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        case JzERHIResourceState::Write:
            return isDepth ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_RENDER_TARGET;
        case JzERHIResourceState::ReadWrite:
            return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        case JzERHIResourceState::Unknown:
            break;
    }

    return D3D12_RESOURCE_STATE_COMMON;
}

D3D12_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(JzEPrimitiveType type)
{
    switch (type) {
        case JzEPrimitiveType::Points:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case JzEPrimitiveType::Lines:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case JzEPrimitiveType::LineStrip:
            return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case JzEPrimitiveType::Triangles:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case JzEPrimitiveType::TriangleStrip:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        case JzEPrimitiveType::TriangleFan:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

    return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

Bool PickHardwareAdapter(IDXGIFactory6 *factory, Microsoft::WRL::ComPtr<IDXGIAdapter1> &outAdapter)
{
    if (!factory) {
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT index = 0; SUCCEEDED(factory->EnumAdapters1(index, &adapter)); ++index) {
        DXGI_ADAPTER_DESC1 desc{};
        if (FAILED(adapter->GetDesc1(&desc))) {
            continue;
        }

        if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) {
            outAdapter = adapter;
            return true;
        }
    }

    return false;
}

} // namespace
JzD3D12Device::JzD3D12Device(JzIWindowBackend &windowBackend) :
    JzDevice(JzERHIType::D3D12),
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
    m_currentClear.stencil      = 0;

    if (!CreateDevice() || !CreateCommandObjects() || !CreateSwapchain() || !CreateRenderTargets() || !CreateDepthBuffer()) {
        JzRE_LOG_ERROR("JzD3D12Device: initialization failed");
        return;
    }

    InitializeCapabilities();

    m_frameIndex = m_swapChain ? m_swapChain->GetCurrentBackBufferIndex() : 0;

    const auto framebufferSize = m_windowBackend->GetFramebufferSize();
    m_currentViewport          = {
        0.0f,
        0.0f,
        static_cast<F32>(std::max<I32>(1, framebufferSize.x)),
        static_cast<F32>(std::max<I32>(1, framebufferSize.y)),
        0.0f,
        1.0f,
    };
    m_currentScissor = {
        0,
        0,
        static_cast<U32>(std::max<I32>(1, framebufferSize.x)),
        static_cast<U32>(std::max<I32>(1, framebufferSize.y)),
    };

    {
        const U32              whitePixel = 0xFFFFFFFFU;
        JzGPUTextureObjectDesc fallbackDesc{};
        fallbackDesc.type      = JzETextureResourceType::Texture2D;
        fallbackDesc.format    = JzETextureResourceFormat::RGBA8;
        fallbackDesc.width     = 1;
        fallbackDesc.height    = 1;
        fallbackDesc.data      = &whitePixel;
        fallbackDesc.debugName = "D3D12FallbackWhite";
        m_fallbackTexture      = std::dynamic_pointer_cast<JzD3D12Texture>(CreateTexture(fallbackDesc));
        if (!m_fallbackTexture) {
            JzRE_LOG_WARN("JzD3D12Device: failed to create fallback texture");
        }
    }

    m_isInitialized = true;
    JzRE_LOG_INFO("JzD3D12Device: initialized");
}

JzD3D12Device::~JzD3D12Device()
{
    Finish();

    m_boundTextures.clear();
    m_fallbackTexture.reset();

    ReleaseSwapchainResources();
    if (m_swapChain) {
        m_swapChain.Reset();
    }

    if (m_commandList) {
        m_commandList.Reset();
    }

    for (auto &frame : m_frames) {
        frame.allocator.Reset();
    }

    if (m_commandQueue) {
        m_commandQueue.Reset();
    }

    if (m_fenceEvent) {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }

    m_fence.Reset();
    m_device.Reset();
    m_factory.Reset();
}

String JzD3D12Device::GetDeviceName() const
{
    if (!m_device) {
        return "Unknown D3D12 Device";
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    if (!PickHardwareAdapter(m_factory.Get(), adapter)) {
        return "Unknown D3D12 Device";
    }

    DXGI_ADAPTER_DESC1 desc{};
    if (FAILED(adapter->GetDesc1(&desc))) {
        return "Unknown D3D12 Device";
    }

    const String name = WideToUtf8(WString(desc.Description));
    if (name.empty()) {
        return "Unknown D3D12 Device";
    }

    return name;
}

String JzD3D12Device::GetVendorName() const
{
    if (!m_device) {
        return "Unknown Vendor";
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    if (!PickHardwareAdapter(m_factory.Get(), adapter)) {
        return "Unknown Vendor";
    }

    DXGI_ADAPTER_DESC1 desc{};
    if (FAILED(adapter->GetDesc1(&desc))) {
        return "Unknown Vendor";
    }

    switch (desc.VendorId) {
        case 0x10DE:
            return "NVIDIA";
        case 0x1002:
        case 0x1022:
            return "AMD";
        case 0x8086:
            return "Intel";
        default:
            break;
    }

    return "Vendor";
}

String JzD3D12Device::GetDriverVersion() const
{
    return "Unknown";
}

std::shared_ptr<JzGPUBufferObject> JzD3D12Device::CreateBuffer(const JzGPUBufferObjectDesc &desc)
{
    auto buffer = std::make_shared<JzD3D12Buffer>(*this, desc);
    m_stats.buffers++;
    return buffer;
}

std::shared_ptr<JzGPUTextureObject> JzD3D12Device::CreateTexture(const JzGPUTextureObjectDesc &desc)
{
    auto texture = std::make_shared<JzD3D12Texture>(*this, desc);
    m_stats.textures++;
    return texture;
}

std::shared_ptr<JzGPUShaderProgramObject> JzD3D12Device::CreateShader(const JzShaderProgramDesc &desc)
{
    auto shader = std::make_shared<JzD3D12Shader>(desc);
    m_stats.shaders++;
    return shader;
}

std::shared_ptr<JzRHIPipeline> JzD3D12Device::CreatePipeline(const JzPipelineDesc &desc)
{
    auto pipeline = std::make_shared<JzD3D12Pipeline>(*this, desc);
    m_stats.pipelines++;
    return pipeline;
}

std::shared_ptr<JzGPUFramebufferObject> JzD3D12Device::CreateFramebuffer(const String &debugName)
{
    return std::make_shared<JzD3D12Framebuffer>(debugName);
}

std::shared_ptr<JzGPUVertexArrayObject> JzD3D12Device::CreateVertexArray(const String &debugName)
{
    return std::make_shared<JzD3D12VertexArray>(debugName);
}

std::shared_ptr<JzRHICommandList> JzD3D12Device::CreateCommandList(const String &debugName)
{
    return std::make_shared<JzRHICommandList>(debugName);
}
void JzD3D12Device::ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList)
{
    if (!commandList || commandList->IsEmpty() || !m_isFrameActive) {
        return;
    }

    const auto commands = commandList->GetCommands();
    for (const auto &command : commands) {
        DispatchCommand(command);
    }
}

void JzD3D12Device::ExecuteCommandLists(const std::vector<std::shared_ptr<JzRHICommandList>> &commandLists)
{
    for (const auto &commandList : commandLists) {
        ExecuteCommandList(commandList);
    }
}

void JzD3D12Device::BeginFrame()
{
    if (!m_isInitialized || !m_device || !m_swapChain) {
        return;
    }

    const auto framebufferSize = m_windowBackend->GetFramebufferSize();
    if (framebufferSize.x <= 0 || framebufferSize.y <= 0) {
        return;
    }

    if (static_cast<U32>(framebufferSize.x) != m_currentScissor.width || static_cast<U32>(framebufferSize.y) != m_currentScissor.height) {
        ResizeSwapchain(static_cast<U32>(framebufferSize.x), static_cast<U32>(framebufferSize.y));
    }

    WaitForFrame(m_frameIndex);

    auto &frame = m_frames[m_frameIndex];
    if (frame.allocator) {
        frame.allocator->Reset();
    }

    if (m_commandList) {
        m_commandList->Reset(frame.allocator.Get(), nullptr);
    }

    auto backBuffer = m_backBuffers[m_frameIndex];
    if (backBuffer) {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource   = backBuffer.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        m_commandList->ResourceBarrier(1, &barrier);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle  = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr                         += static_cast<SIZE_T>(m_frameIndex) * m_rtvDescriptorSize;

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = m_currentViewport.x;
    viewport.TopLeftY = m_currentViewport.y;
    viewport.Width    = m_currentViewport.width;
    viewport.Height   = m_currentViewport.height;
    viewport.MinDepth = m_currentViewport.minDepth;
    viewport.MaxDepth = m_currentViewport.maxDepth;
    m_commandList->RSSetViewports(1, &viewport);

    D3D12_RECT scissor{};
    scissor.left   = m_currentScissor.x;
    scissor.top    = m_currentScissor.y;
    scissor.right  = static_cast<LONG>(m_currentScissor.x + m_currentScissor.width);
    scissor.bottom = static_cast<LONG>(m_currentScissor.y + m_currentScissor.height);
    m_commandList->RSSetScissorRects(1, &scissor);

    m_stats.drawCalls = 0;
    m_stats.triangles = 0;
    m_stats.vertices  = 0;

    m_boundTextures.clear();
    m_isFrameActive   = true;
    m_readyForPresent = false;
}

void JzD3D12Device::EndFrame()
{
    if (!m_isInitialized || !m_isFrameActive) {
        return;
    }

    auto backBuffer = m_backBuffers[m_frameIndex];
    if (backBuffer) {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource   = backBuffer.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        m_commandList->ResourceBarrier(1, &barrier);
    }

    m_commandList->Close();
    m_isFrameActive   = false;
    m_readyForPresent = true;
}

void JzD3D12Device::Flush()
{
    if (!m_isInitialized || !m_readyForPresent) {
        return;
    }

    ID3D12CommandList *lists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(1, lists);

    m_fenceValue++;
    m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
    m_frames[m_frameIndex].fenceValue = m_fenceValue;

    m_swapChain->Present(1, 0);
    m_frameIndex      = m_swapChain->GetCurrentBackBufferIndex();
    m_readyForPresent = false;
}

void JzD3D12Device::Finish()
{
    if (!m_commandQueue || !m_fence) {
        return;
    }

    m_fenceValue++;
    m_commandQueue->Signal(m_fence.Get(), m_fenceValue);

    if (m_fence->GetCompletedValue() < m_fenceValue) {
        m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

Bool JzD3D12Device::SupportsMultithreading() const
{
    return true;
}

ID3D12Device *JzD3D12Device::GetDevice() const
{
    return m_device.Get();
}

ID3D12GraphicsCommandList *JzD3D12Device::GetCommandList() const
{
    return m_commandList.Get();
}

IDXGISwapChain3 *JzD3D12Device::GetSwapChain() const
{
    return m_swapChain.Get();
}

std::shared_ptr<JzD3D12Texture> JzD3D12Device::GetFallbackTexture() const
{
    return m_fallbackTexture;
}

Bool JzD3D12Device::ExecuteImmediate(const std::function<void(ID3D12GraphicsCommandList *)> &recordFn)
{
    if (!m_device || !m_commandQueue || !m_fence || !m_fenceEvent || !recordFn) {
        return false;
    }

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)))) {
        return false;
    }

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) {
        return false;
    }

    recordFn(commandList.Get());
    commandList->Close();

    ID3D12CommandList *lists[] = {commandList.Get()};
    m_commandQueue->ExecuteCommandLists(1, lists);

    m_fenceValue++;
    m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
    if (m_fence->GetCompletedValue() < m_fenceValue) {
        m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    return true;
}

Bool JzD3D12Device::IsFrameRecording() const
{
    return m_isFrameActive;
}

void JzD3D12Device::InitializeCapabilities()
{
    m_capabilities.maxTextureSize                 = 16384;
    m_capabilities.maxTexture3DSize               = 2048;
    m_capabilities.maxTextureArrayLayers          = 2048;
    m_capabilities.maxCubeMapTextureSize          = 16384;
    m_capabilities.maxColorAttachments            = 8;
    m_capabilities.maxRenderTargetSize            = 16384;
    m_capabilities.maxVertexAttributes            = 32;
    m_capabilities.maxUniformBufferBindings       = 64;
    m_capabilities.maxTextureUnits                = 128;
    m_capabilities.maxVertices                    = 0;
    m_capabilities.maxIndices                     = 0;
    m_capabilities.maxSamples                     = 1;
    m_capabilities.supportsComputeShaders         = false;
    m_capabilities.supportsGeometryShaders        = false;
    m_capabilities.supportsTessellationShaders    = false;
    m_capabilities.supportsMultithreadedRendering = true;
    m_capabilities.maxRenderThreads               = 4;
}

Bool JzD3D12Device::CreateDevice()
{
    UINT factoryFlags = 0;

    if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory)))) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create DXGI factory");
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    if (!PickHardwareAdapter(m_factory.Get(), adapter)) {
        Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
        if (FAILED(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)))) {
            JzRE_LOG_ERROR("JzD3D12Device: failed to enumerate WARP adapter");
            return false;
        }
        warpAdapter.As(&adapter);
    }

    if (!adapter) {
        JzRE_LOG_ERROR("JzD3D12Device: no suitable adapter found");
        return false;
    }

    const HRESULT result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
    if (FAILED(result)) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create D3D12 device (HRESULT=0x{:08X})", static_cast<U32>(result));
        return false;
    }

    return true;
}

Bool JzD3D12Device::CreateCommandObjects()
{
    if (!m_device) {
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    if (FAILED(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)))) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create command queue");
        return false;
    }

    for (auto &frame : m_frames) {
        if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.allocator)))) {
            JzRE_LOG_ERROR("JzD3D12Device: failed to create command allocator");
            return false;
        }
    }

    if (FAILED(m_device->CreateCommandList(0,
                                           D3D12_COMMAND_LIST_TYPE_DIRECT,
                                           m_frames[0].allocator.Get(),
                                           nullptr,
                                           IID_PPV_ARGS(&m_commandList)))) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create command list");
        return false;
    }

    m_commandList->Close();

    if (FAILED(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create fence");
        return false;
    }

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_fenceEvent) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create fence event");
        return false;
    }

    return true;
}

Bool JzD3D12Device::CreateSwapchain()
{
    if (!m_factory || !m_commandQueue || !m_windowBackend) {
        return false;
    }

    const auto framebufferSize = m_windowBackend->GetFramebufferSize();
    const U32  width           = static_cast<U32>(std::max<I32>(1, framebufferSize.x));
    const U32  height          = static_cast<U32>(std::max<I32>(1, framebufferSize.y));

    HWND hwnd = reinterpret_cast<HWND>(m_windowBackend->GetNativeWindowHandle());
    if (!hwnd) {
        JzRE_LOG_ERROR("JzD3D12Device: invalid native window handle");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 swapDesc{};
    swapDesc.Width              = width;
    swapDesc.Height             = height;
    swapDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount        = __FRAME_COUNT;
    swapDesc.SampleDesc.Count   = 1;
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapDesc.Scaling            = DXGI_SCALING_STRETCH;
    swapDesc.Flags              = 0;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    const HRESULT                           result = m_factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),
        hwnd,
        &swapDesc,
        nullptr,
        nullptr,
        &swapChain1);

    if (FAILED(result)) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create swapchain (HRESULT=0x{:08X})", static_cast<U32>(result));
        return false;
    }

    m_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    if (FAILED(swapChain1.As(&m_swapChain))) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to acquire swapchain");
        return false;
    }

    return true;
}

Bool JzD3D12Device::CreateRenderTargets()
{
    if (!m_device || !m_swapChain) {
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = __FRAME_COUNT;
    heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask       = 0;

    if (FAILED(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap)))) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create RTV heap");
        return false;
    }

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < __FRAME_COUNT; ++i) {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i])))) {
            JzRE_LOG_ERROR("JzD3D12Device: failed to acquire back buffer {}", i);
            return false;
        }

        m_device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, handle);
        handle.ptr += static_cast<SIZE_T>(m_rtvDescriptorSize);
    }

    return true;
}

Bool JzD3D12Device::CreateDepthBuffer()
{
    if (!m_device || !m_windowBackend) {
        return false;
    }

    const auto framebufferSize = m_windowBackend->GetFramebufferSize();
    const U32  width           = static_cast<U32>(std::max<I32>(1, framebufferSize.x));
    const U32  height          = static_cast<U32>(std::max<I32>(1, framebufferSize.y));

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask       = 0;

    if (FAILED(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsvHeap)))) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create DSV heap");
        return false;
    }

    m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    D3D12_RESOURCE_DESC depthDesc{};
    depthDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Alignment          = 0;
    depthDesc.Width              = width;
    depthDesc.Height             = height;
    depthDesc.DepthOrArraySize   = 1;
    depthDesc.MipLevels          = 1;
    depthDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count   = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clearValue.DepthStencil.Depth   = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type                 = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    const HRESULT result = m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&m_depthBuffer));

    if (FAILED(result)) {
        JzRE_LOG_ERROR("JzD3D12Device: failed to create depth buffer (HRESULT=0x{:08X})", static_cast<U32>(result));
        return false;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;

    m_device->CreateDepthStencilView(m_depthBuffer.Get(),
                                     &dsvDesc,
                                     m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    return true;
}

void JzD3D12Device::ReleaseSwapchainResources()
{
    for (auto &backBuffer : m_backBuffers) {
        if (backBuffer) {
            backBuffer.Reset();
        }
    }

    if (m_depthBuffer) {
        m_depthBuffer.Reset();
    }

    if (m_rtvHeap) {
        m_rtvHeap.Reset();
    }

    if (m_dsvHeap) {
        m_dsvHeap.Reset();
    }

    m_rtvDescriptorSize = 0;
    m_dsvDescriptorSize = 0;
}

Bool JzD3D12Device::ResizeSwapchain(U32 width, U32 height)
{
    if (!m_swapChain || !m_device || width == 0 || height == 0) {
        return false;
    }

    Finish();
    ReleaseSwapchainResources();

    const HRESULT result = m_swapChain->ResizeBuffers(
        __FRAME_COUNT,
        width,
        height,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0);

    if (FAILED(result)) {
        JzRE_LOG_ERROR("JzD3D12Device: swapchain resize failed (HRESULT=0x{:08X})", static_cast<U32>(result));
        return false;
    }

    if (!CreateRenderTargets() || !CreateDepthBuffer()) {
        return false;
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    m_currentViewport.width  = static_cast<F32>(width);
    m_currentViewport.height = static_cast<F32>(height);
    m_currentScissor.width   = width;
    m_currentScissor.height  = height;

    return true;
}

void JzD3D12Device::WaitForFrame(UINT frameIndex)
{
    if (!m_fence || !m_fenceEvent) {
        return;
    }

    const UINT64 fenceValue = m_frames[frameIndex].fenceValue;
    if (fenceValue == 0) {
        return;
    }

    if (m_fence->GetCompletedValue() < fenceValue) {
        m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void JzD3D12Device::DispatchCommand(const JzRHIRecordedCommand &command)
{
    switch (command.type) {
        case JzRHIECommandType::Clear:
        {
            if (const auto *payload = std::get_if<JzClearParams>(&command.payload)) {
                Clear(*payload);
            }
            break;
        }
        case JzRHIECommandType::Draw:
        {
            if (const auto *payload = std::get_if<JzDrawParams>(&command.payload)) {
                Draw(*payload);
            }
            break;
        }
        case JzRHIECommandType::DrawIndexed:
        {
            if (const auto *payload = std::get_if<JzDrawIndexedParams>(&command.payload)) {
                DrawIndexed(*payload);
            }
            break;
        }
        case JzRHIECommandType::BindPipeline:
        {
            if (const auto *payload = std::get_if<JzRHIBindPipelinePayload>(&command.payload)) {
                BindPipeline(payload->pipeline);
            }
            break;
        }
        case JzRHIECommandType::BindVertexArray:
        {
            if (const auto *payload = std::get_if<JzRHIBindVertexArrayPayload>(&command.payload)) {
                BindVertexArray(payload->vertexArray);
            }
            break;
        }
        case JzRHIECommandType::BindTexture:
        {
            if (const auto *payload = std::get_if<JzRHIBindTexturePayload>(&command.payload)) {
                BindTexture(payload->texture, payload->slot);
            }
            break;
        }
        case JzRHIECommandType::BindFramebuffer:
        {
            if (const auto *payload = std::get_if<JzRHIBindFramebufferPayload>(&command.payload)) {
                BindFramebuffer(payload->framebuffer);
            }
            break;
        }
        case JzRHIECommandType::SetViewport:
        {
            if (const auto *payload = std::get_if<JzViewport>(&command.payload)) {
                SetViewport(*payload);
            }
            break;
        }
        case JzRHIECommandType::SetScissor:
        {
            if (const auto *payload = std::get_if<JzScissorRect>(&command.payload)) {
                SetScissor(*payload);
            }
            break;
        }
        case JzRHIECommandType::BeginRenderPass:
        {
            if (const auto *payload = std::get_if<JzRHIBeginRenderPassPayload>(&command.payload)) {
                BindFramebuffer(payload->framebuffer);
                if (payload->renderPass) {
                    payload->renderPass->OnBegin(*this, payload->framebuffer);
                }
            }
            break;
        }
        case JzRHIECommandType::EndRenderPass:
        {
            if (const auto *payload = std::get_if<JzRHIEndRenderPassPayload>(&command.payload)) {
                if (payload->renderPass) {
                    payload->renderPass->OnEnd(*this);
                }
            }
            break;
        }
        case JzRHIECommandType::ResourceBarrier:
        {
            if (const auto *payload = std::get_if<JzRHIResourceBarrierPayload>(&command.payload)) {
                ResourceBarrier(payload->barriers);
            }
            break;
        }
        case JzRHIECommandType::BlitFramebufferToScreen:
        {
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

void JzD3D12Device::BindPipeline(std::shared_ptr<JzRHIPipeline> pipeline)
{
    m_currentPipeline = std::dynamic_pointer_cast<JzD3D12Pipeline>(std::move(pipeline));
}

void JzD3D12Device::BindVertexArray(std::shared_ptr<JzGPUVertexArrayObject> vertexArray)
{
    m_currentVertexArray = std::dynamic_pointer_cast<JzD3D12VertexArray>(std::move(vertexArray));
}

void JzD3D12Device::BindTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 slot)
{
    if (!texture) {
        m_boundTextures.erase(slot);
        return;
    }

    auto d3dTexture = std::dynamic_pointer_cast<JzD3D12Texture>(std::move(texture));
    if (!d3dTexture) {
        m_boundTextures.erase(slot);
        return;
    }

    m_boundTextures[slot] = std::move(d3dTexture);
}

void JzD3D12Device::BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer)
{
    m_currentFramebuffer = std::dynamic_pointer_cast<JzD3D12Framebuffer>(std::move(framebuffer));
}

void JzD3D12Device::SetViewport(const JzViewport &viewport)
{
    m_currentViewport = viewport;

    if (!m_isFrameActive || !m_commandList) {
        return;
    }

    D3D12_VIEWPORT vp{};
    vp.TopLeftX = viewport.x;
    vp.TopLeftY = viewport.y;
    vp.Width    = viewport.width;
    vp.Height   = viewport.height;
    vp.MinDepth = viewport.minDepth;
    vp.MaxDepth = viewport.maxDepth;
    m_commandList->RSSetViewports(1, &vp);
}

void JzD3D12Device::SetScissor(const JzScissorRect &scissor)
{
    m_currentScissor = scissor;

    if (!m_isFrameActive || !m_commandList) {
        return;
    }

    D3D12_RECT rect{};
    rect.left   = scissor.x;
    rect.top    = scissor.y;
    rect.right  = static_cast<LONG>(scissor.x + scissor.width);
    rect.bottom = static_cast<LONG>(scissor.y + scissor.height);
    m_commandList->RSSetScissorRects(1, &rect);
}

void JzD3D12Device::Clear(const JzClearParams &params)
{
    if (!m_isFrameActive || !m_commandList) {
        return;
    }

    m_currentClear = params;

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle  = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr                         += static_cast<SIZE_T>(m_frameIndex) * m_rtvDescriptorSize;

    if (params.clearColor) {
        const FLOAT color[4] = {params.colorR, params.colorG, params.colorB, params.colorA};
        m_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
    }

    if (params.clearDepth || params.clearStencil) {
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_CLEAR_FLAGS           flags     = D3D12_CLEAR_FLAG_DEPTH;
        if (params.clearStencil) {
            flags = static_cast<D3D12_CLEAR_FLAGS>(flags | D3D12_CLEAR_FLAG_STENCIL);
        }
        m_commandList->ClearDepthStencilView(dsvHandle, flags, params.depth, static_cast<UINT8>(params.stencil), 0, nullptr);
    }
}

void JzD3D12Device::Draw(const JzDrawParams &params)
{
    if (!m_isFrameActive || !m_commandList || params.vertexCount == 0) {
        return;
    }

    if (!m_currentPipeline || !m_currentPipeline->IsValid()) {
        return;
    }

    m_commandList->SetPipelineState(m_currentPipeline->GetPipelineState());
    m_currentPipeline->BindResources(m_commandList.Get(), m_boundTextures);
    m_commandList->IASetPrimitiveTopology(ConvertPrimitiveTopology(params.primitiveType));

    if (m_currentVertexArray) {
        for (const auto &binding : m_currentPipeline->GetVertexBindings()) {
            auto iter = m_currentVertexArray->GetVertexBuffers().find(binding.binding);
            if (iter == m_currentVertexArray->GetVertexBuffers().end()) {
                continue;
            }
            auto buffer = std::dynamic_pointer_cast<JzD3D12Buffer>(iter->second);
            if (!buffer || !buffer->GetResource()) {
                continue;
            }

            D3D12_VERTEX_BUFFER_VIEW view{};
            view.BufferLocation = buffer->GetGPUAddress();
            view.SizeInBytes    = static_cast<UINT>(buffer->GetSize());
            view.StrideInBytes  = binding.stride;
            m_commandList->IASetVertexBuffers(binding.binding, 1, &view);
        }
    }

    m_commandList->DrawInstanced(params.vertexCount, params.instanceCount, params.firstVertex, params.firstInstance);

    m_stats.drawCalls++;
    m_stats.vertices += params.vertexCount;
    if (params.primitiveType == JzEPrimitiveType::Triangles) {
        m_stats.triangles += params.vertexCount / 3;
    }
}

void JzD3D12Device::DrawIndexed(const JzDrawIndexedParams &params)
{
    if (!m_isFrameActive || !m_commandList || params.indexCount == 0) {
        return;
    }

    if (!m_currentPipeline || !m_currentPipeline->IsValid()) {
        return;
    }

    m_commandList->SetPipelineState(m_currentPipeline->GetPipelineState());
    m_currentPipeline->BindResources(m_commandList.Get(), m_boundTextures);
    m_commandList->IASetPrimitiveTopology(ConvertPrimitiveTopology(params.primitiveType));

    if (m_currentVertexArray) {
        for (const auto &binding : m_currentPipeline->GetVertexBindings()) {
            auto iter = m_currentVertexArray->GetVertexBuffers().find(binding.binding);
            if (iter == m_currentVertexArray->GetVertexBuffers().end()) {
                continue;
            }
            auto buffer = std::dynamic_pointer_cast<JzD3D12Buffer>(iter->second);
            if (!buffer || !buffer->GetResource()) {
                continue;
            }

            D3D12_VERTEX_BUFFER_VIEW view{};
            view.BufferLocation = buffer->GetGPUAddress();
            view.SizeInBytes    = static_cast<UINT>(buffer->GetSize());
            view.StrideInBytes  = binding.stride;
            m_commandList->IASetVertexBuffers(binding.binding, 1, &view);
        }

        auto indexBuffer = std::dynamic_pointer_cast<JzD3D12Buffer>(m_currentVertexArray->GetIndexBuffer());
        if (!indexBuffer || !indexBuffer->GetResource()) {
            return;
        }

        D3D12_INDEX_BUFFER_VIEW indexView{};
        indexView.BufferLocation = indexBuffer->GetGPUAddress();
        indexView.SizeInBytes    = static_cast<UINT>(indexBuffer->GetSize());
        indexView.Format         = DXGI_FORMAT_R32_UINT;
        m_commandList->IASetIndexBuffer(&indexView);
    }

    m_commandList->DrawIndexedInstanced(params.indexCount, params.instanceCount, params.firstIndex, params.vertexOffset, params.firstInstance);

    m_stats.drawCalls++;
    m_stats.vertices += params.indexCount;
    if (params.primitiveType == JzEPrimitiveType::Triangles) {
        m_stats.triangles += params.indexCount / 3;
    }
}

void JzD3D12Device::ResourceBarrier(const std::vector<JzRHIResourceBarrier> &barriers)
{
    if (!m_isFrameActive || !m_commandList) {
        return;
    }

    std::vector<D3D12_RESOURCE_BARRIER> nativeBarriers;
    nativeBarriers.reserve(barriers.size());

    for (const auto &barrier : barriers) {
        if (barrier.type != JzEResourceType::Texture || !barrier.resource) {
            continue;
        }

        auto texture = std::dynamic_pointer_cast<JzD3D12Texture>(barrier.resource);
        if (!texture || !texture->GetResource()) {
            continue;
        }

        const auto oldState = texture->GetState();
        const auto newState = ConvertResourceState(barrier.after, texture->IsDepthTexture());
        if (oldState == newState) {
            continue;
        }

        D3D12_RESOURCE_BARRIER native{};
        native.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        native.Transition.pResource   = texture->GetResource();
        native.Transition.StateBefore = oldState;
        native.Transition.StateAfter  = newState;
        native.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        nativeBarriers.push_back(native);
        texture->SetState(newState);
    }

    if (!nativeBarriers.empty()) {
        m_commandList->ResourceBarrier(static_cast<UINT>(nativeBarriers.size()), nativeBarriers.data());
    }
}

void JzD3D12Device::BlitFramebufferToScreen(std::shared_ptr<JzGPUFramebufferObject> framebuffer,
                                            U32 srcWidth, U32 srcHeight,
                                            U32 dstWidth, U32 dstHeight)
{
    (void)framebuffer;
    (void)srcWidth;
    (void)srcHeight;
    (void)dstWidth;
    (void)dstHeight;
    // No-op for runtime swapchain path.
}

} // namespace JzRE

#endif // _WIN32
