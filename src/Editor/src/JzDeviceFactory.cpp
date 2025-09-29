/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzDeviceFactory.h"
#include <iostream>
#include "JzRE/Graphics/JzOpenGLDevice.h"

std::unique_ptr<JzRE::JzRHIDevice> JzRE::JzDeviceFactory::CreateDevice(JzRE::JzERHIType rhiType)
{
    if (rhiType == JzERHIType::Unknown) {
        rhiType = GetDefaultRHIType();
    }

    switch (rhiType) {
        case JzERHIType::OpenGL:
            return std::make_unique<JzOpenGLDevice>();
        // case JzERHIType::Vulkan:
        //     return std::make_unique<JzVulkanDevice>();
        default:
            std::cerr << "不支持的RHI类型: " << static_cast<int>(rhiType) << std::endl;
            return nullptr;
    }
}

std::vector<JzRE::JzERHIType> JzRE::JzDeviceFactory::GetSupportedRHITypes()
{
    std::vector<JzERHIType> supportedTypes;

    // OpenGL总是支持的（通过GLAD和GLFW）
    supportedTypes.push_back(JzERHIType::OpenGL);

    // Vulkan支持可以在运行时检测
    // TODO: 添加Vulkan支持检测
    // if (IsVulkanSupported()) {
    //     supportedTypes.push_back(ERHIType::Vulkan);
    // }

    return supportedTypes;
}

JzRE::JzERHIType JzRE::JzDeviceFactory::GetDefaultRHIType()
{
    auto supportedTypes = GetSupportedRHITypes();

    // 优先选择Vulkan，回退到OpenGL
    for (auto type : {/*JzERHIType::Vulkan,*/ JzERHIType::OpenGL}) {
        for (auto supported : supportedTypes) {
            if (type == supported) {
                return type;
            }
        }
    }

    return JzERHIType::Unknown;
}

JzRE::Bool JzRE::JzDeviceFactory::IsRHITypeSupported(JzRE::JzERHIType rhiType)
{
    auto supportedTypes = GetSupportedRHITypes();
    for (auto supported : supportedTypes) {
        if (rhiType == supported) {
            return true;
        }
    }
    return false;
}

JzRE::String JzRE::JzDeviceFactory::GetRHITypeName(JzRE::JzERHIType rhiType)
{
    switch (rhiType) {
        case JzERHIType::OpenGL: return "OpenGL";
        case JzERHIType::Vulkan: return "Vulkan";
        case JzERHIType::D3D11: return "Direct3D 11";
        case JzERHIType::D3D12: return "Direct3D 12";
        case JzERHIType::Metal: return "Metal";
        default: return "Unknown";
    }
}
