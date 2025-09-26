/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzRHIFactory.h"
#include <iostream>
#include "JzRE/Graphics/JzOpenGLDevice.h"

// RHIFactory实现
std::unique_ptr<JzRE::JzRHIDevice> JzRE::JzRHIFactory::CreateDevice(JzRE::JzERHIType rhiType)
{
    if (rhiType == JzERHIType::Unknown) {
        rhiType = GetDefaultRHIType();
    }

    switch (rhiType) {
        case JzERHIType::OpenGL:
        // case JzERHIType::Vulkan:
        //     return std::make_unique<JzVulkanDevice>();
        default:
            std::cerr << "不支持的RHI类型: " << static_cast<int>(rhiType) << std::endl;
            return nullptr;
    }
}

std::vector<JzRE::JzERHIType> JzRE::JzRHIFactory::GetSupportedRHITypes()
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

JzRE::JzERHIType JzRE::JzRHIFactory::GetDefaultRHIType()
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

JzRE::Bool JzRE::JzRHIFactory::IsRHITypeSupported(JzRE::JzERHIType rhiType)
{
    auto supportedTypes = GetSupportedRHITypes();
    for (auto supported : supportedTypes) {
        if (rhiType == supported) {
            return true;
        }
    }
    return false;
}

JzRE::String JzRE::JzRHIFactory::GetRHITypeName(JzRE::JzERHIType rhiType)
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
