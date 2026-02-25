/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

#if defined(_WIN32)
#include <wrl/client.h>
#include <d3d12.h>
#endif

namespace JzRE {

#if defined(_WIN32)

class JzD3D12Device;
class JzD3D12Texture;
class JzD3D12Shader;

/**
 * @brief D3D12 uniform member metadata.
 */
struct JzD3D12UniformMember {
    U32 offset = 0;
    U32 size   = 0;
};

/**
 * @brief D3D12 uniform buffer binding.
 */
struct JzD3D12UniformBinding {
    U32                                              set             = 0;
    U32                                              binding         = 0;
    U32                                              size            = 0;
    U32                                              alignedSize     = 0;
    U32                                              descriptorIndex = 0;
    std::vector<U8>                                  cpuData;
    void                                            *mappedData = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource>           buffer;
    std::unordered_map<String, JzD3D12UniformMember> members;
};

/**
 * @brief Descriptor binding entry.
 */
struct JzD3D12DescriptorBinding {
    JzEShaderResourceType type = JzEShaderResourceType::UniformBuffer;
    String                name;
    U32                   set             = 0;
    U32                   binding         = 0;
    U32                   descriptorIndex = 0;
};

/**
 * @brief Direct3D 12 pipeline implementation.
 */
class JzD3D12Pipeline final : public JzRHIPipeline {
public:
    JzD3D12Pipeline(JzD3D12Device &device, const JzPipelineDesc &desc);
    ~JzD3D12Pipeline() override;

    void CommitParameters() override;

    Bool IsValid() const;

    ID3D12PipelineState *GetPipelineState() const;
    ID3D12RootSignature *GetRootSignature() const;

    void BindResources(ID3D12GraphicsCommandList                                      *commandList,
                       const std::unordered_map<U32, std::shared_ptr<JzD3D12Texture>> &boundTextures);

    const std::vector<JzVertexBindingDesc> &GetVertexBindings() const;

private:
    Bool BuildRootSignature();
    Bool BuildPipelineState();
    Bool BuildReflection();
    void UploadUniformParameters();
    void UpdateTextureDescriptors(const std::unordered_map<U32, std::shared_ptr<JzD3D12Texture>> &boundTextures);

private:
    JzD3D12Device *m_owner   = nullptr;
    Bool           m_isValid = false;

    struct JzD3D12Semantic {
        String name;
        U32    index = 0;
    };

    std::vector<std::shared_ptr<JzD3D12Shader>> m_shaders;
    std::unordered_map<U32, JzD3D12Semantic>    m_inputSemantics;
    std::vector<String>                         m_semanticNames;
    std::vector<D3D12_INPUT_ELEMENT_DESC>       m_inputElements;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

    std::vector<JzD3D12UniformBinding>    m_uniformBindings;
    std::vector<JzD3D12DescriptorBinding> m_resourceBindings;
    std::vector<JzD3D12DescriptorBinding> m_samplerBindings;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvSrvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
    UINT                                         m_cbvSrvDescriptorSize  = 0;
    UINT                                         m_samplerDescriptorSize = 0;

    std::vector<JzVertexBindingDesc> m_vertexBindings;
    JzVertexLayoutDesc               m_vertexLayout;
};

#endif // _WIN32

} // namespace JzRE
