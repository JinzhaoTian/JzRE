/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

#if defined(_WIN32)
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#endif

namespace JzRE {

#if defined(_WIN32)

class JzD3D12Device;

/**
 * @brief Direct3D 12 implementation of GPU texture object.
 */
class JzD3D12Texture final : public JzGPUTextureObject {
public:
    /**
     * @brief Construct a D3D12 texture.
     *
     * @param device Owner D3D12 device.
     * @param desc Texture description.
     */
    JzD3D12Texture(JzD3D12Device &device, const JzGPUTextureObjectDesc &desc);

    /**
     * @brief Destructor.
     */
    ~JzD3D12Texture() override;

    /**
     * @brief Update texture data.
     */
    void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) override;

    /**
     * @brief Generate mipmaps (not supported in minimal D3D12 backend).
     */
    void GenerateMipmaps() override;

    /**
     * @brief Get native texture token (resource pointer).
     */
    void *GetTextureID() const override;

    /**
     * @brief Get the native D3D12 resource.
     */
    ID3D12Resource *GetResource() const;

    /**
     * @brief Get the DXGI format.
     */
    DXGI_FORMAT GetFormat() const;

    /**
     * @brief Get current resource state.
     */
    D3D12_RESOURCE_STATES GetState() const;

    /**
     * @brief Update current resource state.
     */
    void SetState(D3D12_RESOURCE_STATES state);

    /**
     * @brief Whether the texture is depth/stencil.
     */
    Bool IsDepthTexture() const;

    /**
     * @brief Create shader resource view descriptor.
     */
    void CreateShaderResourceView(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

    /**
     * @brief Create sampler descriptor.
     */
    void CreateSampler(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

    /**
     * @brief Create render target view descriptor.
     */
    void CreateRenderTargetView(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

    /**
     * @brief Create depth stencil view descriptor.
     */
    void CreateDepthStencilView(ID3D12Device *device, D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

private:
    JzD3D12Device                         *m_owner = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
    DXGI_FORMAT                            m_format = DXGI_FORMAT_UNKNOWN;
    D3D12_RESOURCE_STATES                  m_state  = D3D12_RESOURCE_STATE_COMMON;
    D3D12_SAMPLER_DESC                     m_samplerDesc{};
    Bool                                   m_isDepth = false;
};

#endif // _WIN32

} // namespace JzRE
