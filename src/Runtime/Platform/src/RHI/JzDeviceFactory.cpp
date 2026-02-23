/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/RHI/JzDeviceFactory.h"

#include <algorithm>
#include <cstring>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/OpenGL/JzOpenGLDevice.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"
#include "JzRE/Runtime/Platform/Window/JzIWindowBackend.h"

namespace {

JzRE::Bool HasRequiredInstanceExtensions()
{
    uint32_t      requiredCount      = 0;
    const char **requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredCount);
    if (!requiredExtensions || requiredCount == 0) {
        return false;
    }

    uint32_t resultCount = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &resultCount, nullptr) != VK_SUCCESS) {
        return false;
    }

    std::vector<VkExtensionProperties> available(resultCount);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &resultCount, available.data()) != VK_SUCCESS) {
        return false;
    }

    for (uint32_t i = 0; i < requiredCount; ++i) {
        const char *required = requiredExtensions[i];
        const JzRE::Bool found = std::any_of(
            available.begin(), available.end(),
            [required](const VkExtensionProperties &prop) {
                return std::strcmp(prop.extensionName, required) == 0;
            });
        if (!found) {
            return false;
        }
    }

    return true;
}

} // namespace

std::unique_ptr<JzRE::JzDevice> JzRE::JzDeviceFactory::CreateDevice(JzRE::JzERHIType rhiType, JzIWindowBackend *windowBackend)
{
    const auto requestedType = rhiType;
    auto       selectedType  = rhiType;
    if (selectedType == JzERHIType::Unknown) {
        selectedType = GetDefaultRHIType();
    }

    if (selectedType == JzERHIType::Vulkan && !IsVulkanSupported()) {
        JzRE_LOG_WARN("JzDeviceFactory: Vulkan requested but unavailable, falling back to OpenGL");
        selectedType = JzERHIType::OpenGL;
    }

    if (selectedType == JzERHIType::Vulkan && !windowBackend) {
        JzRE_LOG_WARN("JzDeviceFactory: Vulkan requires a window backend, falling back to OpenGL");
        selectedType = JzERHIType::OpenGL;
    }

    JzRE_LOG_INFO("JzDeviceFactory: requested RHI '{}', selected RHI '{}'",
                  GetRHITypeName(requestedType), GetRHITypeName(selectedType));

    switch (selectedType) {
        case JzERHIType::OpenGL:
            return std::make_unique<JzOpenGLDevice>();
        case JzERHIType::Vulkan:
            return std::make_unique<JzVulkanDevice>(*windowBackend);
        default:
            JzRE_LOG_WARN("JzDeviceFactory: Unsupported RHI type '{}', forcing OpenGL",
                          GetRHITypeName(selectedType));
            return std::make_unique<JzOpenGLDevice>();
    }
}

std::vector<JzRE::JzERHIType> JzRE::JzDeviceFactory::GetSupportedRHITypes()
{
    std::vector<JzERHIType> supportedTypes;

    if (IsVulkanSupported()) {
        supportedTypes.push_back(JzERHIType::Vulkan);
    }

    supportedTypes.push_back(JzERHIType::OpenGL);

    return supportedTypes;
}

JzRE::JzERHIType JzRE::JzDeviceFactory::GetDefaultRHIType()
{
    const auto supportedTypes = GetSupportedRHITypes();

    const std::vector<JzERHIType> preferredTypes = {
        JzERHIType::Vulkan,
        JzERHIType::OpenGL};

    for (auto type : preferredTypes) {
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
    if (rhiType == JzERHIType::Unknown) {
        return false;
    }

    const auto supportedTypes = GetSupportedRHITypes();
    for (auto supported : supportedTypes) {
        if (rhiType == supported) {
            return true;
        }
    }
    return false;
}

JzRE::Bool JzRE::JzDeviceFactory::IsVulkanSupported()
{
    if (glfwInit() != GLFW_TRUE) {
        return false;
    }

    if (glfwVulkanSupported() != GLFW_TRUE) {
        return false;
    }

    if (!HasRequiredInstanceExtensions()) {
        return false;
    }

    return true;
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
