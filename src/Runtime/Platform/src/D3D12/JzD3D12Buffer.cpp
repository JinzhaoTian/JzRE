/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#if defined(_WIN32)
#define NOMINMAX
#endif

#include "JzRE/Runtime/Platform/D3D12/JzD3D12Buffer.h"

#if defined(_WIN32)

#include <algorithm>
#include <cstring>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Device.h"

namespace JzRE {

namespace {

D3D12_RESOURCE_DESC MakeBufferDesc(Size size)
{
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment          = 0;
    desc.Width              = static_cast<UINT64>(std::max<Size>(size, 1));
    desc.Height             = 1;
    desc.DepthOrArraySize   = 1;
    desc.MipLevels          = 1;
    desc.Format             = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
    return desc;
}

} // namespace

JzD3D12Buffer::JzD3D12Buffer(JzD3D12Device &device, const JzGPUBufferObjectDesc &desc) :
    JzGPUBufferObject(desc),
    m_owner(&device)
{
    ID3D12Device *d3dDevice = device.GetDevice();
    if (!d3dDevice) {
        return;
    }

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type                 = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    const D3D12_RESOURCE_DESC bufferDesc = MakeBufferDesc(desc.size);

    const HRESULT result = d3dDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_resource));
    if (FAILED(result)) {
        JzRE_LOG_ERROR("JzD3D12Buffer: failed to create buffer resource (HRESULT=0x{:08X})", static_cast<U32>(result));
        return;
    }

    m_state = D3D12_RESOURCE_STATE_GENERIC_READ;

    if (desc.data && desc.size > 0) {
        UpdateData(desc.data, desc.size, 0);
    }
}

JzD3D12Buffer::~JzD3D12Buffer()
{
    if (m_mapped && m_resource) {
        m_resource->Unmap(0, nullptr);
        m_mapped = nullptr;
    }
}

void JzD3D12Buffer::UpdateData(const void *data, Size size, Size offset)
{
    if (!data || size == 0 || !m_resource) {
        return;
    }

    if (offset + size > desc.size) {
        JzRE_LOG_WARN("JzD3D12Buffer: update range exceeds buffer size (offset={}, size={}, capacity={})",
                      offset,
                      size,
                      desc.size);
        return;
    }

    void *mapped = MapBuffer();
    if (!mapped) {
        return;
    }

    std::memcpy(static_cast<U8 *>(mapped) + offset, data, size);
}

void *JzD3D12Buffer::MapBuffer()
{
    if (!m_resource) {
        return nullptr;
    }

    if (!m_mapped) {
        const HRESULT result = m_resource->Map(0, nullptr, &m_mapped);
        if (FAILED(result)) {
            JzRE_LOG_ERROR("JzD3D12Buffer: failed to map buffer (HRESULT=0x{:08X})", static_cast<U32>(result));
            m_mapped = nullptr;
        }
    }

    return m_mapped;
}

void JzD3D12Buffer::UnmapBuffer()
{
    if (!m_resource || !m_mapped) {
        return;
    }

    m_resource->Unmap(0, nullptr);
    m_mapped = nullptr;
}

ID3D12Resource *JzD3D12Buffer::GetResource() const
{
    return m_resource.Get();
}

D3D12_GPU_VIRTUAL_ADDRESS JzD3D12Buffer::GetGPUAddress() const
{
    if (!m_resource) {
        return 0;
    }
    return m_resource->GetGPUVirtualAddress();
}

} // namespace JzRE

#endif // _WIN32
