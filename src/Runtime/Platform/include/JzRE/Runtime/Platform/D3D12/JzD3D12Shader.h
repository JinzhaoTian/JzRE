/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/Runtime/Platform/RHI/JzGPUShaderProgramObject.h"

#if defined(_WIN32)
#include <d3d12.h>
#endif

namespace JzRE {

#if defined(_WIN32)

/**
 * @brief Direct3D 12 shader object created from DXIL payload.
 */
class JzD3D12Shader final : public JzGPUShaderProgramObject {
public:
    /**
     * @brief Construct a D3D12 shader object.
     *
     * @param desc Shader program description.
     */
    explicit JzD3D12Shader(const JzShaderProgramDesc &desc);

    /**
     * @brief Destructor.
     */
    ~JzD3D12Shader() override = default;

    /**
     * @brief Check if shader payload is valid.
     */
    Bool IsCompiled() const;

    /**
     * @brief Get compile log if validation fails.
     */
    const String &GetCompileLog() const;

    /**
     * @brief Get D3D12 shader bytecode.
     */
    D3D12_SHADER_BYTECODE GetShaderBytecode() const;

private:
    Bool   m_isValid = false;
    String m_compileLog;
};

#endif // _WIN32

} // namespace JzRE
