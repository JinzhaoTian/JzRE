/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/D3D12/JzD3D12Shader.h"

#if defined(_WIN32)

namespace JzRE {

JzD3D12Shader::JzD3D12Shader(const JzShaderProgramDesc &desc) :
    JzGPUShaderProgramObject(desc)
{
    if (desc.format != JzEShaderBinaryFormat::DXIL) {
        m_compileLog = "D3D12 shader requires DXIL payload";
        m_isValid    = false;
        return;
    }

    if (desc.bytecodeOrText.empty()) {
        m_compileLog = "D3D12 shader payload is empty";
        m_isValid    = false;
        return;
    }

    m_isValid = true;
}

Bool JzD3D12Shader::IsCompiled() const
{
    return m_isValid;
}

const String &JzD3D12Shader::GetCompileLog() const
{
    return m_compileLog;
}

D3D12_SHADER_BYTECODE JzD3D12Shader::GetShaderBytecode() const
{
    D3D12_SHADER_BYTECODE bytecode{};
    if (!m_isValid || desc.bytecodeOrText.empty()) {
        return bytecode;
    }

    bytecode.pShaderBytecode = desc.bytecodeOrText.data();
    bytecode.BytecodeLength  = desc.bytecodeOrText.size();
    return bytecode;
}

} // namespace JzRE

#endif // _WIN32
