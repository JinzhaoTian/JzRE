/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzGPUResource.h"

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
 * @brief Shader program description
 */
struct JzShaderProgramDesc {
    JzEShaderProgramType type;
    String               source;
    String               entryPoint = "main";
    String               debugName;
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
        return desc.type;
    }

    /**
     * @brief Get the source of the shader
     *
     * @return The source of the shader
     */
    const String &GetSource() const
    {
        return desc.source;
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

protected:
    JzShaderProgramDesc desc;
};

} // namespace JzRE