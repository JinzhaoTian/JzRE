
/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#if defined(_WIN32)
#define NOMINMAX
#endif

#include "JzRE/Runtime/Platform/D3D12/JzD3D12Pipeline.h"

#if defined(_WIN32)

#include <algorithm>
#include <cstring>
#include <unordered_map>

#include <d3d12shader.h>
#include <dxcapi.h>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Device.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Shader.h"
#include "JzRE/Runtime/Platform/D3D12/JzD3D12Texture.h"

namespace JzRE {

namespace {

struct JzReflectedCBuffer {
    U32                                              size = 0;
    std::unordered_map<String, JzD3D12UniformMember> members;
};

U32 AlignTo(U32 value, U32 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

U32 ComputeTypeSize(const D3D12_SHADER_TYPE_DESC &desc)
{
    const U32 elements = desc.Elements == 0 ? 1 : desc.Elements;

    switch (desc.Class) {
        case D3D_SVC_MATRIX_ROWS:
            return 16 * desc.Rows * elements;
        case D3D_SVC_MATRIX_COLUMNS:
            return 16 * desc.Columns * elements;
        case D3D_SVC_VECTOR:
        case D3D_SVC_SCALAR:
            return 4 * std::max(1u, desc.Columns) * elements;
        default:
            break;
    }

    return 4 * std::max(1u, desc.Columns) * elements;
}

DXGI_FORMAT ConvertVertexFormat(JzEVertexAttributeFormat format)
{
    switch (format) {
        case JzEVertexAttributeFormat::Float:
            return DXGI_FORMAT_R32_FLOAT;
        case JzEVertexAttributeFormat::Float2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case JzEVertexAttributeFormat::Float3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case JzEVertexAttributeFormat::Float4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case JzEVertexAttributeFormat::Int:
            return DXGI_FORMAT_R32_SINT;
        case JzEVertexAttributeFormat::Int2:
            return DXGI_FORMAT_R32G32_SINT;
        case JzEVertexAttributeFormat::Int3:
            return DXGI_FORMAT_R32G32B32_SINT;
        case JzEVertexAttributeFormat::Int4:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case JzEVertexAttributeFormat::UInt:
            return DXGI_FORMAT_R32_UINT;
        case JzEVertexAttributeFormat::UInt2:
            return DXGI_FORMAT_R32G32_UINT;
        case JzEVertexAttributeFormat::UInt3:
            return DXGI_FORMAT_R32G32B32_UINT;
        case JzEVertexAttributeFormat::UInt4:
            return DXGI_FORMAT_R32G32B32A32_UINT;
    }

    return DXGI_FORMAT_R32G32B32_FLOAT;
}

D3D12_CULL_MODE ConvertCullMode(JzECullMode mode)
{
    switch (mode) {
        case JzECullMode::None:
            return D3D12_CULL_MODE_NONE;
        case JzECullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        case JzECullMode::Back:
            return D3D12_CULL_MODE_BACK;
        case JzECullMode::FrontAndBack:
            return D3D12_CULL_MODE_BACK;
    }
    return D3D12_CULL_MODE_BACK;
}

D3D12_COMPARISON_FUNC ConvertDepthFunc(JzEDepthFunc func)
{
    switch (func) {
        case JzEDepthFunc::Never:
            return D3D12_COMPARISON_FUNC_NEVER;
        case JzEDepthFunc::Less:
            return D3D12_COMPARISON_FUNC_LESS;
        case JzEDepthFunc::Equal:
            return D3D12_COMPARISON_FUNC_EQUAL;
        case JzEDepthFunc::LessEqual:
            return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case JzEDepthFunc::Greater:
            return D3D12_COMPARISON_FUNC_GREATER;
        case JzEDepthFunc::NotEqual:
            return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case JzEDepthFunc::GreaterEqual:
            return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case JzEDepthFunc::Always:
            return D3D12_COMPARISON_FUNC_ALWAYS;
    }
    return D3D12_COMPARISON_FUNC_LESS;
}

void ConfigureBlend(JzEBlendMode mode, D3D12_BLEND_DESC &blendDesc)
{
    blendDesc.AlphaToCoverageEnable  = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;

    auto &rt                 = blendDesc.RenderTarget[0];
    rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    rt.BlendEnable           = FALSE;
    rt.SrcBlend              = D3D12_BLEND_ONE;
    rt.DestBlend             = D3D12_BLEND_ZERO;
    rt.BlendOp               = D3D12_BLEND_OP_ADD;
    rt.SrcBlendAlpha         = D3D12_BLEND_ONE;
    rt.DestBlendAlpha        = D3D12_BLEND_ZERO;
    rt.BlendOpAlpha          = D3D12_BLEND_OP_ADD;

    switch (mode) {
        case JzEBlendMode::None:
            break;
        case JzEBlendMode::Alpha:
            rt.BlendEnable    = TRUE;
            rt.SrcBlend       = D3D12_BLEND_SRC_ALPHA;
            rt.DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
            rt.SrcBlendAlpha  = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            break;
        case JzEBlendMode::Additive:
            rt.BlendEnable    = TRUE;
            rt.SrcBlend       = D3D12_BLEND_SRC_ALPHA;
            rt.DestBlend      = D3D12_BLEND_ONE;
            rt.SrcBlendAlpha  = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ONE;
            break;
        case JzEBlendMode::Multiply:
            rt.BlendEnable    = TRUE;
            rt.SrcBlend       = D3D12_BLEND_DEST_COLOR;
            rt.DestBlend      = D3D12_BLEND_ZERO;
            rt.SrcBlendAlpha  = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ZERO;
            break;
    }
}

Microsoft::WRL::ComPtr<ID3D12ShaderReflection> ReflectShader(const std::vector<U8> &bytecode)
{
    if (bytecode.empty()) {
        return nullptr;
    }

    Microsoft::WRL::ComPtr<IDxcUtils> utils;
    if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)))) {
        return nullptr;
    }

    Microsoft::WRL::ComPtr<IDxcContainerReflection> reflection;
    if (FAILED(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&reflection)))) {
        return nullptr;
    }

    Microsoft::WRL::ComPtr<IDxcBlobEncoding> blob;
    if (FAILED(utils->CreateBlobFromPinned(bytecode.data(), static_cast<UINT32>(bytecode.size()), DXC_CP_ACP, &blob))) {
        return nullptr;
    }

    if (FAILED(reflection->Load(blob.Get()))) {
        return nullptr;
    }

    UINT32 dxilIndex = 0;
    if (FAILED(reflection->FindFirstPartKind(DXC_PART_DXIL, &dxilIndex))) {
        return nullptr;
    }

    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shaderReflection;
    if (FAILED(reflection->GetPartReflection(dxilIndex, IID_PPV_ARGS(&shaderReflection)))) {
        return nullptr;
    }

    return shaderReflection;
}

void CollectMembers(ID3D12ShaderReflectionType *type, U32 baseOffset, const String &prefix,
                    std::unordered_map<String, JzD3D12UniformMember> &outMembers)
{
    if (!type) {
        return;
    }

    D3D12_SHADER_TYPE_DESC typeDesc{};
    if (FAILED(type->GetDesc(&typeDesc))) {
        return;
    }

    if (typeDesc.Members > 0) {
        for (UINT i = 0; i < typeDesc.Members; ++i) {
            auto *memberType = type->GetMemberTypeByIndex(i);
            if (!memberType) {
                continue;
            }

            D3D12_SHADER_TYPE_DESC memberDesc{};
            if (FAILED(memberType->GetDesc(&memberDesc))) {
                continue;
            }

            const char *memberName = type->GetMemberTypeName(i);
            String      nextPrefix = prefix;
            if (memberName && *memberName != '\0') {
                if (!nextPrefix.empty()) {
                    nextPrefix += ".";
                }
                nextPrefix += memberName;
            }
            const UINT memberOffset = memberDesc.Offset;
            CollectMembers(memberType, baseOffset + memberOffset, nextPrefix, outMembers);
        }
        return;
    }

    if (prefix.empty()) {
        return;
    }

    JzD3D12UniformMember member;
    member.offset      = baseOffset;
    member.size        = ComputeTypeSize(typeDesc);
    outMembers[prefix] = member;
}

} // namespace
JzD3D12Pipeline::JzD3D12Pipeline(JzD3D12Device &device, const JzPipelineDesc &desc) :
    JzRHIPipeline(desc),
    m_owner(&device),
    m_vertexBindings(desc.vertexLayout.bindings),
    m_vertexLayout(desc.vertexLayout)
{
    m_isValid = true;

    for (const auto &shaderDesc : desc.shaders) {
        auto shader = std::make_shared<JzD3D12Shader>(shaderDesc);
        if (!shader->IsCompiled()) {
            JzRE_LOG_ERROR("JzD3D12Pipeline: invalid shader stage '{}' for pipeline '{}': {}",
                           static_cast<I32>(shaderDesc.stage),
                           desc.debugName,
                           shader->GetCompileLog());
            m_isValid = false;
            continue;
        }
        m_shaders.push_back(std::move(shader));
    }

    if (m_shaders.empty()) {
        m_isValid = false;
        return;
    }

    if (!BuildReflection()) {
        m_isValid = false;
        return;
    }

    if (!BuildRootSignature()) {
        m_isValid = false;
        return;
    }

    if (!BuildPipelineState()) {
        m_isValid = false;
        return;
    }
}

JzD3D12Pipeline::~JzD3D12Pipeline() = default;

void JzD3D12Pipeline::CommitParameters()
{
    UploadUniformParameters();
}

Bool JzD3D12Pipeline::IsValid() const
{
    return m_isValid;
}

ID3D12PipelineState *JzD3D12Pipeline::GetPipelineState() const
{
    return m_pipelineState.Get();
}

ID3D12RootSignature *JzD3D12Pipeline::GetRootSignature() const
{
    return m_rootSignature.Get();
}

const std::vector<JzVertexBindingDesc> &JzD3D12Pipeline::GetVertexBindings() const
{
    return m_vertexBindings;
}

Bool JzD3D12Pipeline::BuildReflection()
{
    m_inputSemantics.clear();

    std::unordered_map<U64, JzReflectedCBuffer> reflectedBuffers;

    for (const auto &shader : m_shaders) {
        const auto bytecode   = shader->GetPayload();
        auto       reflection = ReflectShader(bytecode);
        if (!reflection) {
            continue;
        }

        D3D12_SHADER_DESC shaderDesc{};
        if (FAILED(reflection->GetDesc(&shaderDesc))) {
            continue;
        }

        if (shader->GetType() == JzEShaderProgramType::Vertex) {
            for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
                D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
                if (FAILED(reflection->GetInputParameterDesc(i, &paramDesc))) {
                    continue;
                }

                if (paramDesc.SemanticName) {
                    JzD3D12Semantic semantic;
                    semantic.name                        = paramDesc.SemanticName;
                    semantic.index                       = paramDesc.SemanticIndex;
                    m_inputSemantics[paramDesc.Register] = semantic;
                }
            }
        }

        std::unordered_map<String, std::pair<U32, U32>> cbufferBindings;
        for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
            if (FAILED(reflection->GetResourceBindingDesc(i, &bindDesc))) {
                continue;
            }

            if (bindDesc.Type == D3D_SIT_CBUFFER && bindDesc.Name) {
                cbufferBindings[bindDesc.Name] = {bindDesc.Space, bindDesc.BindPoint};
            }
        }

        for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i) {
            auto *cbuffer = reflection->GetConstantBufferByIndex(i);
            if (!cbuffer) {
                continue;
            }

            D3D12_SHADER_BUFFER_DESC bufferDesc{};
            if (FAILED(cbuffer->GetDesc(&bufferDesc)) || !bufferDesc.Name) {
                continue;
            }

            auto bindingIter = cbufferBindings.find(bufferDesc.Name);
            if (bindingIter == cbufferBindings.end()) {
                continue;
            }

            const U32 space   = bindingIter->second.first;
            const U32 binding = bindingIter->second.second;
            const U64 key     = (static_cast<U64>(space) << 32) | binding;

            auto &reflected = reflectedBuffers[key];
            reflected.size  = std::max<U32>(reflected.size, bufferDesc.Size);

            for (UINT v = 0; v < bufferDesc.Variables; ++v) {
                auto *var = cbuffer->GetVariableByIndex(v);
                if (!var) {
                    continue;
                }

                D3D12_SHADER_VARIABLE_DESC varDesc{};
                if (FAILED(var->GetDesc(&varDesc)) || !varDesc.Name) {
                    continue;
                }

                auto *type = var->GetType();
                CollectMembers(type, varDesc.StartOffset, varDesc.Name, reflected.members);
            }
        }
    }

    m_uniformBindings.clear();
    for (const auto &resource : desc.shaderLayout.resources) {
        if (resource.type != JzEShaderResourceType::UniformBuffer) {
            continue;
        }

        const U64 key  = (static_cast<U64>(resource.set) << 32) | resource.binding;
        auto      iter = reflectedBuffers.find(key);
        if (iter == reflectedBuffers.end()) {
            continue;
        }

        JzD3D12UniformBinding binding;
        binding.set         = resource.set;
        binding.binding     = resource.binding;
        binding.size        = iter->second.size;
        binding.alignedSize = AlignTo(binding.size, 256);
        binding.members     = iter->second.members;
        binding.cpuData.assign(binding.alignedSize, 0);
        m_uniformBindings.push_back(std::move(binding));
    }

    return true;
}
Bool JzD3D12Pipeline::BuildRootSignature()
{
    if (!m_owner || !m_owner->GetDevice()) {
        return false;
    }

    m_resourceBindings.clear();
    m_samplerBindings.clear();

    std::vector<D3D12_DESCRIPTOR_RANGE> cbvSrvRanges;
    std::vector<D3D12_DESCRIPTOR_RANGE> samplerRanges;
    cbvSrvRanges.reserve(desc.shaderLayout.resources.size());
    samplerRanges.reserve(desc.shaderLayout.resources.size());

    UINT cbvSrvIndex  = 0;
    UINT samplerIndex = 0;

    for (const auto &resource : desc.shaderLayout.resources) {
        if (resource.type == JzEShaderResourceType::PushConstants) {
            continue;
        }

        if (resource.type == JzEShaderResourceType::Sampler) {
            D3D12_DESCRIPTOR_RANGE range{};
            range.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            range.NumDescriptors                    = 1;
            range.BaseShaderRegister                = resource.binding;
            range.RegisterSpace                     = resource.set;
            range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            samplerRanges.push_back(range);

            JzD3D12DescriptorBinding binding{};
            binding.type            = resource.type;
            binding.name            = resource.name;
            binding.set             = resource.set;
            binding.binding         = resource.binding;
            binding.descriptorIndex = samplerIndex++;
            m_samplerBindings.push_back(binding);
            continue;
        }

        D3D12_DESCRIPTOR_RANGE range{};
        range.RangeType                         = (resource.type == JzEShaderResourceType::UniformBuffer) ? D3D12_DESCRIPTOR_RANGE_TYPE_CBV : D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range.NumDescriptors                    = 1;
        range.BaseShaderRegister                = resource.binding;
        range.RegisterSpace                     = resource.set;
        range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        cbvSrvRanges.push_back(range);

        JzD3D12DescriptorBinding binding{};
        binding.type            = resource.type;
        binding.name            = resource.name;
        binding.set             = resource.set;
        binding.binding         = resource.binding;
        binding.descriptorIndex = cbvSrvIndex++;
        m_resourceBindings.push_back(binding);
    }

    std::vector<D3D12_ROOT_PARAMETER> rootParams;
    rootParams.reserve(2);

    if (!cbvSrvRanges.empty()) {
        D3D12_ROOT_PARAMETER param{};
        param.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvSrvRanges.size());
        param.DescriptorTable.pDescriptorRanges   = cbvSrvRanges.data();
        param.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
        rootParams.push_back(param);
    }

    if (!samplerRanges.empty()) {
        D3D12_ROOT_PARAMETER param{};
        param.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(samplerRanges.size());
        param.DescriptorTable.pDescriptorRanges   = samplerRanges.data();
        param.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
        rootParams.push_back(param);
    }

    D3D12_ROOT_SIGNATURE_DESC rootDesc{};
    rootDesc.NumParameters     = static_cast<UINT>(rootParams.size());
    rootDesc.pParameters       = rootParams.empty() ? nullptr : rootParams.data();
    rootDesc.NumStaticSamplers = 0;
    rootDesc.pStaticSamplers   = nullptr;
    rootDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    const HRESULT                    serializeResult = D3D12SerializeRootSignature(
        &rootDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signatureBlob,
        &errorBlob);

    if (FAILED(serializeResult)) {
        if (errorBlob) {
            const char *msg = static_cast<const char *>(errorBlob->GetBufferPointer());
            JzRE_LOG_ERROR("JzD3D12Pipeline: root signature serialization failed: {}", msg ? msg : "Unknown");
        } else {
            JzRE_LOG_ERROR("JzD3D12Pipeline: root signature serialization failed (HRESULT=0x{:08X})", static_cast<U32>(serializeResult));
        }
        return false;
    }

    const HRESULT createResult = m_owner->GetDevice()->CreateRootSignature(
        0,
        signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature));

    if (FAILED(createResult)) {
        JzRE_LOG_ERROR("JzD3D12Pipeline: failed to create root signature (HRESULT=0x{:08X})", static_cast<U32>(createResult));
        return false;
    }

    if (!m_resourceBindings.empty()) {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.NumDescriptors = static_cast<UINT>(m_resourceBindings.size());
        heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDesc.NodeMask       = 0;

        const HRESULT heapResult = m_owner->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvSrvHeap));
        if (FAILED(heapResult)) {
            JzRE_LOG_ERROR("JzD3D12Pipeline: failed to create CBV/SRV heap (HRESULT=0x{:08X})", static_cast<U32>(heapResult));
            return false;
        }

        m_cbvSrvDescriptorSize = m_owner->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    if (!m_samplerBindings.empty()) {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        heapDesc.NumDescriptors = static_cast<UINT>(m_samplerBindings.size());
        heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDesc.NodeMask       = 0;

        const HRESULT heapResult = m_owner->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_samplerHeap));
        if (FAILED(heapResult)) {
            JzRE_LOG_ERROR("JzD3D12Pipeline: failed to create sampler heap (HRESULT=0x{:08X})", static_cast<U32>(heapResult));
            return false;
        }

        m_samplerDescriptorSize = m_owner->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    }

    for (auto &uniform : m_uniformBindings) {
        auto iter = std::find_if(m_resourceBindings.begin(), m_resourceBindings.end(), [&uniform](const JzD3D12DescriptorBinding &binding) {
            return binding.type == JzEShaderResourceType::UniformBuffer && binding.set == uniform.set && binding.binding == uniform.binding;
        });
        if (iter != m_resourceBindings.end()) {
            uniform.descriptorIndex = iter->descriptorIndex;
        }
    }

    if (m_cbvSrvHeap && !m_uniformBindings.empty()) {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cbvSrvHeap->GetCPUDescriptorHandleForHeapStart();
        for (auto &uniform : m_uniformBindings) {
            D3D12_HEAP_PROPERTIES heapProps{};
            heapProps.Type                 = D3D12_HEAP_TYPE_UPLOAD;
            heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            D3D12_RESOURCE_DESC bufferDesc{};
            bufferDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
            bufferDesc.Alignment          = 0;
            bufferDesc.Width              = std::max<U32>(uniform.alignedSize, 256);
            bufferDesc.Height             = 1;
            bufferDesc.DepthOrArraySize   = 1;
            bufferDesc.MipLevels          = 1;
            bufferDesc.Format             = DXGI_FORMAT_UNKNOWN;
            bufferDesc.SampleDesc.Count   = 1;
            bufferDesc.SampleDesc.Quality = 0;
            bufferDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            bufferDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

            const HRESULT bufferResult = m_owner->GetDevice()->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&uniform.buffer));

            if (FAILED(bufferResult)) {
                JzRE_LOG_ERROR("JzD3D12Pipeline: failed to create uniform buffer (HRESULT=0x{:08X})", static_cast<U32>(bufferResult));
                continue;
            }

            uniform.buffer->Map(0, nullptr, &uniform.mappedData);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = uniform.buffer->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes    = uniform.alignedSize;

            D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle  = handle;
            cbvHandle.ptr                         += static_cast<SIZE_T>(uniform.descriptorIndex) * m_cbvSrvDescriptorSize;
            m_owner->GetDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);
        }
    }

    if (m_samplerHeap && !m_samplerBindings.empty()) {
        auto                        fallbackTexture = m_owner->GetFallbackTexture();
        D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle   = m_samplerHeap->GetCPUDescriptorHandleForHeapStart();
        for (const auto &binding : m_samplerBindings) {
            D3D12_CPU_DESCRIPTOR_HANDLE handle  = samplerHandle;
            handle.ptr                         += static_cast<SIZE_T>(binding.descriptorIndex) * m_samplerDescriptorSize;
            if (fallbackTexture) {
                fallbackTexture->CreateSampler(m_owner->GetDevice(), handle);
            }
        }
    }

    return true;
}
Bool JzD3D12Pipeline::BuildPipelineState()
{
    if (!m_owner || !m_rootSignature) {
        return false;
    }

    m_inputElements.clear();
    m_semanticNames.clear();

    if (m_vertexLayout.IsValid()) {
        for (const auto &attribute : m_vertexLayout.attributes) {
            D3D12_INPUT_ELEMENT_DESC element{};

            auto semanticIter = m_inputSemantics.find(attribute.location);
            if (semanticIter != m_inputSemantics.end()) {
                m_semanticNames.push_back(semanticIter->second.name);
                element.SemanticName  = m_semanticNames.back().c_str();
                element.SemanticIndex = semanticIter->second.index;
            } else {
                m_semanticNames.push_back("TEXCOORD");
                element.SemanticName  = m_semanticNames.back().c_str();
                element.SemanticIndex = attribute.location;
            }

            element.Format               = ConvertVertexFormat(attribute.format);
            element.InputSlot            = attribute.binding;
            element.AlignedByteOffset    = attribute.offset;
            element.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            element.InstanceDataStepRate = 0;

            auto bindingIter = std::find_if(m_vertexLayout.bindings.begin(), m_vertexLayout.bindings.end(),
                                            [&attribute](const JzVertexBindingDesc &binding) {
                                                return binding.binding == attribute.binding;
                                            });
            if (bindingIter != m_vertexLayout.bindings.end() && bindingIter->perInstance) {
                element.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                element.InstanceDataStepRate = 1;
            }

            m_inputElements.push_back(element);
        }
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature = m_rootSignature.Get();

    for (const auto &shader : m_shaders) {
        if (shader->GetType() == JzEShaderProgramType::Vertex) {
            psoDesc.VS = shader->GetShaderBytecode();
        } else if (shader->GetType() == JzEShaderProgramType::Fragment) {
            psoDesc.PS = shader->GetShaderBytecode();
        }
    }

    psoDesc.InputLayout.pInputElementDescs = m_inputElements.empty() ? nullptr : m_inputElements.data();
    psoDesc.InputLayout.NumElements        = static_cast<UINT>(m_inputElements.size());

    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    psoDesc.RasterizerState.FillMode              = desc.renderState.wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode              = ConvertCullMode(desc.renderState.cullMode);
    psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
    psoDesc.RasterizerState.DepthClipEnable       = TRUE;

    ConfigureBlend(desc.renderState.blendMode, psoDesc.BlendState);

    psoDesc.DepthStencilState.DepthEnable      = desc.renderState.depthTest ? TRUE : FALSE;
    psoDesc.DepthStencilState.DepthWriteMask   = desc.renderState.depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    psoDesc.DepthStencilState.DepthFunc        = ConvertDepthFunc(desc.renderState.depthFunc);
    psoDesc.DepthStencilState.StencilEnable    = FALSE;
    psoDesc.DepthStencilState.StencilReadMask  = D3D12_DEFAULT_STENCIL_READ_MASK;
    psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0]    = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.SampleMask       = UINT_MAX;
    psoDesc.SampleDesc.Count = 1;

    const HRESULT result = m_owner->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
    if (FAILED(result)) {
        JzRE_LOG_ERROR("JzD3D12Pipeline: failed to create pipeline state (HRESULT=0x{:08X})", static_cast<U32>(result));
        return false;
    }

    return true;
}

void JzD3D12Pipeline::UploadUniformParameters()
{
    if (m_uniformBindings.empty()) {
        if (HasDirtyParameters()) {
            MarkParametersCommitted();
        }
        return;
    }

    if (!HasDirtyParameters()) {
        return;
    }

    const auto &parameters = GetParameterCache();

    auto writeBytes = [](std::vector<U8> &buffer, U32 offset, const void *src, U32 bytes) {
        if (!src || bytes == 0 || offset >= buffer.size()) {
            return;
        }
        const U32 available = static_cast<U32>(buffer.size() - offset);
        const U32 copySize  = std::min(bytes, available);
        std::memcpy(buffer.data() + offset, src, copySize);
    };

    for (auto &uniform : m_uniformBindings) {
        if (!uniform.buffer || !uniform.mappedData) {
            continue;
        }

        if (uniform.cpuData.size() != uniform.alignedSize) {
            uniform.cpuData.assign(uniform.alignedSize, 0);
        } else {
            std::fill(uniform.cpuData.begin(), uniform.cpuData.end(), 0);
        }

        for (const auto &[name, value] : parameters) {
            const auto memberIter = uniform.members.find(name);
            if (memberIter == uniform.members.end()) {
                continue;
            }

            const auto &member = memberIter->second;
            std::visit(
                [&](const auto &typedValue) {
                    using TValue = std::decay_t<decltype(typedValue)>;
                    if constexpr (std::is_same_v<TValue, I32>) {
                        writeBytes(uniform.cpuData, member.offset, &typedValue, std::min<U32>(member.size, sizeof(I32)));
                    } else if constexpr (std::is_same_v<TValue, F32>) {
                        writeBytes(uniform.cpuData, member.offset, &typedValue, std::min<U32>(member.size, sizeof(F32)));
                    } else if constexpr (std::is_same_v<TValue, JzVec2>) {
                        writeBytes(uniform.cpuData, member.offset, typedValue.Data(), std::min<U32>(member.size, sizeof(F32) * 2));
                    } else if constexpr (std::is_same_v<TValue, JzVec3>) {
                        writeBytes(uniform.cpuData, member.offset, typedValue.Data(), std::min<U32>(member.size, sizeof(F32) * 3));
                    } else if constexpr (std::is_same_v<TValue, JzVec4>) {
                        writeBytes(uniform.cpuData, member.offset, typedValue.Data(), std::min<U32>(member.size, sizeof(F32) * 4));
                    } else if constexpr (std::is_same_v<TValue, JzMat3>) {
                        const JzMat3 transposed = typedValue.Transpose();
                        if (member.size >= 48) {
                            const F32 *data = transposed.Data();
                            for (U32 column = 0; column < 3; ++column) {
                                writeBytes(uniform.cpuData,
                                           member.offset + column * 16,
                                           data + column * 3,
                                           sizeof(F32) * 3);
                            }
                        } else {
                            writeBytes(uniform.cpuData, member.offset, transposed.Data(), std::min<U32>(member.size, sizeof(F32) * 9));
                        }
                    } else if constexpr (std::is_same_v<TValue, JzMat4>) {
                        const JzMat4 transposed = typedValue.Transpose();
                        writeBytes(uniform.cpuData, member.offset, transposed.Data(), std::min<U32>(member.size, sizeof(F32) * 16));
                    }
                },
                value);
        }

        std::memcpy(uniform.mappedData, uniform.cpuData.data(), uniform.alignedSize);
    }

    MarkParametersCommitted();
}

void JzD3D12Pipeline::UpdateTextureDescriptors(const std::unordered_map<U32, std::shared_ptr<JzD3D12Texture>> &boundTextures)
{
    if (!m_cbvSrvHeap || m_resourceBindings.empty()) {
        return;
    }

    const auto &parameters      = GetParameterCache();
    auto        fallbackTexture = m_owner->GetFallbackTexture();

    const auto ResolveSlot = [&parameters](const String &parameterName) -> U32 {
        auto toSlot = [](const JzShaderParameterValue &value) -> U32 {
            U32 slot = 0;
            std::visit(
                [&slot](const auto &typedValue) {
                    using TValue = std::decay_t<decltype(typedValue)>;
                    if constexpr (std::is_same_v<TValue, I32>) {
                        slot = typedValue < 0 ? 0 : static_cast<U32>(typedValue);
                    } else if constexpr (std::is_same_v<TValue, F32>) {
                        slot = typedValue < 0.0f ? 0 : static_cast<U32>(typedValue);
                    }
                },
                value);
            return slot;
        };

        auto iter = parameters.find(parameterName);
        if (iter != parameters.end()) {
            return toSlot(iter->second);
        }

        if (parameterName.size() > 7 && parameterName.ends_with("Sampler")) {
            const String baseName = parameterName.substr(0, parameterName.size() - 7);
            iter                  = parameters.find(baseName);
            if (iter != parameters.end()) {
                return toSlot(iter->second);
            }
        }

        return 0;
    };

    auto heapStart = m_cbvSrvHeap->GetCPUDescriptorHandleForHeapStart();

    for (const auto &binding : m_resourceBindings) {
        if (binding.type != JzEShaderResourceType::SampledTexture && binding.type != JzEShaderResourceType::StorageTexture) {
            continue;
        }

        const U32                       slot = ResolveSlot(binding.name);
        std::shared_ptr<JzD3D12Texture> texture;
        auto                            boundIter = boundTextures.find(slot);
        if (boundIter != boundTextures.end() && boundIter->second) {
            texture = boundIter->second;
        } else {
            texture = fallbackTexture;
        }

        if (!texture) {
            continue;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE handle  = heapStart;
        handle.ptr                         += static_cast<SIZE_T>(binding.descriptorIndex) * m_cbvSrvDescriptorSize;
        texture->CreateShaderResourceView(m_owner->GetDevice(), handle);
    }

    if (!m_samplerHeap || m_samplerBindings.empty()) {
        return;
    }

    auto samplerStart = m_samplerHeap->GetCPUDescriptorHandleForHeapStart();
    for (const auto &binding : m_samplerBindings) {
        const U32                       slot = ResolveSlot(binding.name);
        std::shared_ptr<JzD3D12Texture> texture;
        auto                            boundIter = boundTextures.find(slot);
        if (boundIter != boundTextures.end() && boundIter->second) {
            texture = boundIter->second;
        } else {
            texture = fallbackTexture;
        }

        if (!texture) {
            continue;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE handle  = samplerStart;
        handle.ptr                         += static_cast<SIZE_T>(binding.descriptorIndex) * m_samplerDescriptorSize;
        texture->CreateSampler(m_owner->GetDevice(), handle);
    }
}

void JzD3D12Pipeline::BindResources(ID3D12GraphicsCommandList                                      *commandList,
                                    const std::unordered_map<U32, std::shared_ptr<JzD3D12Texture>> &boundTextures)
{
    if (!commandList || !m_rootSignature) {
        return;
    }

    UploadUniformParameters();
    UpdateTextureDescriptors(boundTextures);

    std::vector<ID3D12DescriptorHeap *> heaps;
    if (m_cbvSrvHeap) {
        heaps.push_back(m_cbvSrvHeap.Get());
    }
    if (m_samplerHeap) {
        heaps.push_back(m_samplerHeap.Get());
    }

    if (!heaps.empty()) {
        commandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());
    }

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    UINT rootIndex = 0;
    if (m_cbvSrvHeap) {
        commandList->SetGraphicsRootDescriptorTable(rootIndex, m_cbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
        ++rootIndex;
    }

    if (m_samplerHeap) {
        commandList->SetGraphicsRootDescriptorTable(rootIndex, m_samplerHeap->GetGPUDescriptorHandleForHeapStart());
    }
}

} // namespace JzRE

#endif // _WIN32
