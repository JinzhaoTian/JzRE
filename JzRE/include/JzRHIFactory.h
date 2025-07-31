#pragma once

#include "CommonTypes.h"
#include "JzRHIDevice.h"
#include "JzRHIETypes.h"

namespace JzRE {
/**
 * RHI工厂类
 * 负责创建不同图形API的RHI设备
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