#pragma once

#include "CommonTypes.h"
#include "JzRHIDevice.h"
#include "JzRHIETypes.h"

namespace JzRE {
/**
 * @brief RHI Factory
 */
class JzRHIFactory {
public:
    static std::unique_ptr<JzRHIDevice> CreateDevice(JzERHIType rhiType);
    static std::vector<JzERHIType>      GetSupportedRHITypes();
    static JzERHIType                   GetDefaultRHIType();
    static Bool                         IsRHITypeSupported(JzERHIType rhiType);
    static String                       GetRHITypeName(JzERHIType rhiType);

private:
    JzRHIFactory() = default;
};
} // namespace JzRE