#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
/**
 * @brief Interface of RHI Shader
 */
class JzRHIShader : public JzRHIResource {
public:
    JzRHIShader(const JzShaderDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHIShader() = default;

    JzEShaderType GetType() const
    {
        return desc.type;
    }
    const String &GetSource() const
    {
        return desc.source;
    }
    const String &GetEntryPoint() const
    {
        return desc.entryPoint;
    }

protected:
    JzShaderDesc desc;
};
} // namespace JzRE