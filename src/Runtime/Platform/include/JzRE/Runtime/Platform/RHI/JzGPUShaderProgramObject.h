/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <utility>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUResource.h"

namespace JzRE {

/**
 * @brief Enums of shader program types
 */
enum class JzEShaderProgramType : U8 {
    Vertex,
    Fragment,
    Geometry,
    TessellationControl,
    TessellationEvaluation,
    Compute
};

/**
 * @brief Serialized shader payload format.
 *
 * GLSL/MSL are text payloads.
 * SPIRV/DXIL are binary payloads.
 */
enum class JzEShaderBinaryFormat : U8 {
    DXIL,
    SPIRV,
    MSL,
    GLSL
};

/**
 * @brief Shader program description
 */
struct JzShaderProgramDesc {
    JzEShaderProgramType  stage  = JzEShaderProgramType::Vertex;
    JzEShaderBinaryFormat format = JzEShaderBinaryFormat::GLSL;
    std::vector<U8>       bytecodeOrText;
    String                entryPoint = "main";
    String                debugName;
    String                reflectionKey;

    /**
     * @brief Assign UTF-8 shader text into payload storage.
     */
    void SetTextPayload(const String &text)
    {
        bytecodeOrText.assign(text.begin(), text.end());
    }

    /**
     * @brief Decode payload as UTF-8 shader text.
     */
    String GetTextPayload() const
    {
        return String(bytecodeOrText.begin(), bytecodeOrText.end());
    }

    /**
     * @brief Assign binary payload.
     */
    void SetBinaryPayload(std::vector<U8> binary)
    {
        bytecodeOrText = std::move(binary);
    }

    /**
     * @brief Whether payload should be treated as text.
     */
    Bool IsTextPayload() const
    {
        return format == JzEShaderBinaryFormat::GLSL || format == JzEShaderBinaryFormat::MSL;
    }
};

/**
 * @brief Interface of GPU shader program object
 */
class JzGPUShaderProgramObject : public JzGPUResource {
public:
    /**
     * @brief Constructor
     *
     * @param desc The description of the shader
     */
    JzGPUShaderProgramObject(const JzShaderProgramDesc &desc) :
        JzGPUResource(desc.debugName), desc(desc) { }

    /**
     * @brief Destructor
     */
    virtual ~JzGPUShaderProgramObject() = default;

    /**
     * @brief Get the type of the shader
     *
     * @return The type of the shader
     */
    JzEShaderProgramType GetType() const
    {
        return desc.stage;
    }

    /**
     * @brief Get the shader payload format.
     *
     * @return Payload format.
     */
    JzEShaderBinaryFormat GetFormat() const
    {
        return desc.format;
    }

    /**
     * @brief Get the shader payload bytes.
     *
     * @return Binary/text payload bytes.
     */
    const std::vector<U8> &GetPayload() const
    {
        return desc.bytecodeOrText;
    }

    /**
     * @brief Get text payload decoded as UTF-8.
     */
    String GetTextPayload() const
    {
        return desc.GetTextPayload();
    }

    /**
     * @brief Get the entry point of the shader
     *
     * @return The entry point of the shader
     */
    const String &GetEntryPoint() const
    {
        return desc.entryPoint;
    }

    /**
     * @brief Get reflection key for backend layout binding.
     */
    const String &GetReflectionKey() const
    {
        return desc.reflectionKey;
    }

protected:
    JzShaderProgramDesc desc;
};

} // namespace JzRE
