#pragma once

#include "CommonTypes.h"
#include "JzRHIDesc.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
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