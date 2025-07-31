#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {

/**
 * RHI着色器接口
 */
class JzRHIShader : public JzRHIResource {
public:
    JzRHIShader(const ShaderDesc &desc) :
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