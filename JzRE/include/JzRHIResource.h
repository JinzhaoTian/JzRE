#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"

namespace JzRE {

/**
 * RHI资源基类
 */
class JzRHIResource {
public:
    JzRHIResource(const String &debugName = "") :
        debugName(debugName) { }
    virtual ~JzRHIResource() = default;

    const String &GetDebugName() const
    {
        return debugName;
    }
    void SetDebugName(const String &name)
    {
        debugName = name;
    }

protected:
    String debugName;
};

} // namespace JzRE