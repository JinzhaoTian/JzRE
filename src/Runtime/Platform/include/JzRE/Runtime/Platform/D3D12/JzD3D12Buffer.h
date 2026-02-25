/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"

#if defined(_WIN32)
#include <wrl/client.h>
#include <d3d12.h>
#endif

namespace JzRE {

#if defined(_WIN32)

class JzD3D12Device;

/**
 * @brief Direct3D 12 implementation of GPU buffer object.
 */
class JzD3D12Buffer final : public JzGPUBufferObject {
public:
    /**
     * @brief Construct a D3D12 buffer.
     *
     * @param device Owner D3D12 device.
     * @param desc Buffer description.
     */
    JzD3D12Buffer(JzD3D12Device &device, const JzGPUBufferObjectDesc &desc);

    /**
     * @brief Destructor.
     */
    ~JzD3D12Buffer() override;

    /**
     * @brief Update buffer data.
     */
    void UpdateData(const void *data, Size size, Size offset = 0) override;

    /**
     * @brief Map the buffer.
     */
    void *MapBuffer() override;

    /**
     * @brief Unmap the buffer.
     */
    void UnmapBuffer() override;

    /**
     * @brief Get the native D3D12 resource.
     */
    ID3D12Resource *GetResource() const;

    /**
     * @brief Get the GPU virtual address.
     */
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const;

private:
    JzD3D12Device                         *m_owner = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
    D3D12_RESOURCE_STATES                  m_state  = D3D12_RESOURCE_STATE_COMMON;
    void                                  *m_mapped = nullptr;
};

#endif // _WIN32

} // namespace JzRE
