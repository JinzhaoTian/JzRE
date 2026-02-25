/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#if defined(_WIN32)
#define NOMINMAX
#endif

#include "JzRE/Runtime/Platform/D3D12/JzD3D12Texture.h"

#if defined(_WIN32)

#include <algorithm>
#include <cstring>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Device.h"

namespace JzRE {

namespace {

Bool IsDepthFormat(JzETextureResourceFormat format)
{
    return format == JzETextureResourceFormat::Depth16 || format == JzETextureResourceFormat::Depth24 || format == JzETextureResourceFormat::Depth32F || format == JzETextureResourceFormat::Depth24Stencil8;
}

DXGI_FORMAT ConvertFormat(JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::R8:
            return DXGI_FORMAT_R8_UNORM;
        case JzETextureResourceFormat::RG8:
            return DXGI_FORMAT_R8G8_UNORM;
        case JzETextureResourceFormat::RGB8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case JzETextureResourceFormat::RGBA8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case JzETextureResourceFormat::R16F:
            return DXGI_FORMAT_R16_FLOAT;
        case JzETextureResourceFormat::RG16F:
            return DXGI_FORMAT_R16G16_FLOAT;
        case JzETextureResourceFormat::RGB16F:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case JzETextureResourceFormat::RGBA16F:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case JzETextureResourceFormat::R32F:
            return DXGI_FORMAT_R32_FLOAT;
        case JzETextureResourceFormat::RG32F:
            return DXGI_FORMAT_R32G32_FLOAT;
        case JzETextureResourceFormat::RGB32F:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case JzETextureResourceFormat::RGBA32F:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case JzETextureResourceFormat::Depth16:
            return DXGI_FORMAT_D16_UNORM;
        case JzETextureResourceFormat::Depth24:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case JzETextureResourceFormat::Depth32F:
            return DXGI_FORMAT_D32_FLOAT;
        case JzETextureResourceFormat::Depth24Stencil8:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default:
            break;
    }

    return DXGI_FORMAT_R8G8B8A8_UNORM;
}

U32 GetPixelSize(JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::R8:
            return 1;
        case JzETextureResourceFormat::RG8:
            return 2;
        case JzETextureResourceFormat::RGB8:
            return 4;
        case JzETextureResourceFormat::RGBA8:
            return 4;
        case JzETextureResourceFormat::R16F:
            return 2;
        case JzETextureResourceFormat::RG16F:
            return 4;
        case JzETextureResourceFormat::RGB16F:
            return 8;
        case JzETextureResourceFormat::RGBA16F:
            return 8;
        case JzETextureResourceFormat::R32F:
            return 4;
        case JzETextureResourceFormat::RG32F:
            return 8;
        case JzETextureResourceFormat::RGB32F:
            return 16;
        case JzETextureResourceFormat::RGBA32F:
            return 16;
        default:
            return 0;
    }
}

} // namespace

JzD3D12Texture::JzD3D12Texture(JzD3D12Device &device, const JzGPUTextureObjectDesc &desc) :
    JzGPUTextureObject(desc),
    m_owner(&device)
{
    ID3D12Device *d3dDevice = device.GetDevice();
    if (!d3dDevice) {
        return;
    }

    m_isDepth = IsDepthFormat(desc.format);
    m_format  = ConvertFormat(desc.format);

    D3D12_RESOURCE_DESC texDesc{};
    texDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment          = 0;
    texDesc.Width              = std::max<U32>(1, desc.width);
    texDesc.Height             = std::max<U32>(1, desc.height);
    texDesc.DepthOrArraySize   = static_cast<UINT16>(std::max<U32>(1, desc.arraySize));
    texDesc.MipLevels          = static_cast<UINT16>(std::max<U32>(1, desc.mipLevels));
    texDesc.Format             = m_format;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags              = m_isDepth ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = m_format;
    if (m_isDepth) {
        clearValue.DepthStencil.Depth   = 1.0f;
        clearValue.DepthStencil.Stencil = 0;
    } else {
        clearValue.Color[0] = 0.0f;
        clearValue.Color[1] = 0.0f;
        clearValue.Color[2] = 0.0f;
        clearValue.Color[3] = 1.0f;
    }

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type                 = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    const D3D12_RESOURCE_STATES initialState = m_isDepth ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    const HRESULT result = d3dDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        desc.data ? D3D12_RESOURCE_STATE_COPY_DEST : initialState,
        m_isDepth ? &clearValue : nullptr,
        IID_PPV_ARGS(&m_resource));

    if (FAILED(result)) {
        JzRE_LOG_ERROR("JzD3D12Texture: failed to create texture resource (HRESULT=0x{:08X})", static_cast<U32>(result));
        return;
    }

    m_state = desc.data ? D3D12_RESOURCE_STATE_COPY_DEST : initialState;

    m_samplerDesc.Filter         = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    m_samplerDesc.AddressU       = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.AddressV       = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.AddressW       = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.MipLODBias     = 0.0f;
    m_samplerDesc.MaxAnisotropy  = 1;
    m_samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    m_samplerDesc.MinLOD         = 0.0f;
    m_samplerDesc.MaxLOD         = D3D12_FLOAT32_MAX;

    if (desc.data) {
        UpdateData(desc.data, 0, 0);
    }
}

JzD3D12Texture::~JzD3D12Texture() = default;

void JzD3D12Texture::UpdateData(const void *data, U32 mipLevel, U32 arrayIndex)
{
    if (!data || !m_owner || !m_resource) {
        return;
    }

    if (mipLevel != 0 || arrayIndex != 0) {
        JzRE_LOG_WARN("JzD3D12Texture: only mipLevel 0 / arrayIndex 0 supported in minimal backend");
        return;
    }

    const U32 pixelSize = GetPixelSize(desc.format);
    if (pixelSize == 0 || m_isDepth) {
        return;
    }

    ID3D12Device *device = m_owner->GetDevice();
    if (!device) {
        return;
    }

    const D3D12_RESOURCE_DESC          texDesc    = m_resource->GetDesc();
    UINT64                             totalBytes = 0;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout{};
    UINT                               numRows = 0;
    UINT64                             rowSize = 0;

    device->GetCopyableFootprints(&texDesc, 0, 1, 0, &layout, &numRows, &rowSize, &totalBytes);
    if (totalBytes == 0) {
        return;
    }

    D3D12_HEAP_PROPERTIES uploadHeap{};
    uploadHeap.Type                 = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeap.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC bufferDesc{};
    bufferDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Alignment          = 0;
    bufferDesc.Width              = totalBytes;
    bufferDesc.Height             = 1;
    bufferDesc.DepthOrArraySize   = 1;
    bufferDesc.MipLevels          = 1;
    bufferDesc.Format             = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count   = 1;
    bufferDesc.SampleDesc.Quality = 0;
    bufferDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

    Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
    const HRESULT                          uploadResult = device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadResource));

    if (FAILED(uploadResult)) {
        JzRE_LOG_ERROR("JzD3D12Texture: failed to create upload buffer (HRESULT=0x{:08X})", static_cast<U32>(uploadResult));
        return;
    }

    void *mapped = nullptr;
    if (FAILED(uploadResource->Map(0, nullptr, &mapped)) || !mapped) {
        JzRE_LOG_ERROR("JzD3D12Texture: failed to map upload buffer");
        return;
    }

    const U8  *srcBytes        = static_cast<const U8 *>(data);
    U8        *dstBytes        = static_cast<U8 *>(mapped) + layout.Offset;
    const UINT rowPitch        = layout.Footprint.RowPitch;
    const UINT expectedRowSize = static_cast<UINT>(desc.width * pixelSize);

    for (UINT row = 0; row < numRows; ++row) {
        std::memcpy(dstBytes + rowPitch * row,
                    srcBytes + expectedRowSize * row,
                    std::min(rowSize, static_cast<UINT64>(expectedRowSize)));
    }

    uploadResource->Unmap(0, nullptr);

    m_owner->ExecuteImmediate([this,
                               upload = uploadResource,
                               layout](ID3D12GraphicsCommandList *commandList) {
        if (!commandList) {
            return;
        }

        if (m_state != D3D12_RESOURCE_STATE_COPY_DEST) {
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource   = m_resource.Get();
            barrier.Transition.StateBefore = m_state;
            barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            commandList->ResourceBarrier(1, &barrier);
            m_state = D3D12_RESOURCE_STATE_COPY_DEST;
        }

        D3D12_TEXTURE_COPY_LOCATION dst{};
        dst.pResource        = m_resource.Get();
        dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION src{};
        src.pResource       = upload.Get();
        src.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        src.PlacedFootprint = layout;

        commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

        const D3D12_RESOURCE_STATES finalState = m_isDepth ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if (m_state != finalState) {
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource   = m_resource.Get();
            barrier.Transition.StateBefore = m_state;
            barrier.Transition.StateAfter  = finalState;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            commandList->ResourceBarrier(1, &barrier);
            m_state = finalState;
        }
    });
}

void JzD3D12Texture::GenerateMipmaps()
{
    // Not supported in minimal D3D12 backend.
}

void *JzD3D12Texture::GetTextureID() const
{
    return reinterpret_cast<void *>(m_resource.Get());
}

ID3D12Resource *JzD3D12Texture::GetResource() const
{
    return m_resource.Get();
}

DXGI_FORMAT JzD3D12Texture::GetFormat() const
{
    return m_format;
}

D3D12_RESOURCE_STATES JzD3D12Texture::GetState() const
{
    return m_state;
}

void JzD3D12Texture::SetState(D3D12_RESOURCE_STATES state)
{
    m_state = state;
}

Bool JzD3D12Texture::IsDepthTexture() const
{
    return m_isDepth;
}

void JzD3D12Texture::CreateShaderResourceView(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const
{
    if (!device || !m_resource) {
        return;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Format                    = m_format;
    srv.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MostDetailedMip = 0;
    srv.Texture2D.MipLevels       = m_resource->GetDesc().MipLevels;

    device->CreateShaderResourceView(m_resource.Get(), &srv, handle);
}

void JzD3D12Texture::CreateSampler(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const
{
    if (!device) {
        return;
    }

    device->CreateSampler(&m_samplerDesc, handle);
}

void JzD3D12Texture::CreateRenderTargetView(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const
{
    if (!device || !m_resource || m_isDepth) {
        return;
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtv{};
    rtv.Format               = m_format;
    rtv.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtv.Texture2D.MipSlice   = 0;
    rtv.Texture2D.PlaneSlice = 0;

    device->CreateRenderTargetView(m_resource.Get(), &rtv, handle);
}

void JzD3D12Texture::CreateDepthStencilView(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const
{
    if (!device || !m_resource || !m_isDepth) {
        return;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv{};
    dsv.Format        = m_format;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv.Flags         = D3D12_DSV_FLAG_NONE;

    device->CreateDepthStencilView(m_resource.Get(), &dsv, handle);
}

} // namespace JzRE

#endif // _WIN32
