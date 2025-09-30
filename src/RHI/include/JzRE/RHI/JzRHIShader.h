/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHIResource.h"

namespace JzRE {

/**
 * @brief Shader type
 */
enum class JzEShaderType : U8 {
    Vertex,
    Fragment,
    Geometry,
    TessellationControl,
    TessellationEvaluation,
    Compute
};

/**
 * @brief Shader description
 */
struct JzShaderDesc {
    JzEShaderType type;
    String        source;
    String        entryPoint = "main";
    String        debugName;
};

/**
 * @brief Interface of RHI Shader
 */
class JzRHIShader : public JzRHIResource {
public:
    /**
     * @brief Constructor
     * @param desc The description of the shader
     */
    JzRHIShader(const JzShaderDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIShader() = default;

    /**
     * @brief Get the type of the shader
     * @return The type of the shader
     */
    JzEShaderType GetType() const
    {
        return desc.type;
    }

    /**
     * @brief Get the source of the shader
     * @return The source of the shader
     */
    const String &GetSource() const
    {
        return desc.source;
    }

    /**
     * @brief Get the entry point of the shader
     * @return The entry point of the shader
     */
    const String &GetEntryPoint() const
    {
        return desc.entryPoint;
    }

protected:
    JzShaderDesc desc;
};

} // namespace JzRE