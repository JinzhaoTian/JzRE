#include "RHI.h"
#include "OpenGLRHI.h"
#include "VulkanRHI.h"

namespace JzRE {

// RHIStats实现
void RHIStats::Reset() {
    drawCalls = 0;
    triangles = 0;
    vertices = 0;
    buffers = 0;
    textures = 0;
    shaders = 0;
    pipelines = 0;
    bufferMemory = 0;
    textureMemory = 0;
    totalMemory = 0;
    frameTime = 0.0f;
    gpuTime = 0.0f;
}

// RHIFactory实现
std::unique_ptr<RHIDevice> RHIFactory::CreateDevice(ERHIType rhiType) {
    if (rhiType == ERHIType::Unknown) {
        rhiType = GetDefaultRHIType();
    }

    switch (rhiType) {
        case ERHIType::OpenGL:
            return std::make_unique<OpenGLDevice>();
        case ERHIType::Vulkan:
            return std::make_unique<VulkanDevice>();
        default:
            std::cerr << "不支持的RHI类型: " << static_cast<int>(rhiType) << std::endl;
            return nullptr;
    }
}

std::vector<ERHIType> RHIFactory::GetSupportedRHITypes() {
    std::vector<ERHIType> supportedTypes;
    
    // OpenGL总是支持的（通过GLAD和GLFW）
    supportedTypes.push_back(ERHIType::OpenGL);
    
    // Vulkan支持可以在运行时检测
    // TODO: 添加Vulkan支持检测
    // if (IsVulkanSupported()) {
    //     supportedTypes.push_back(ERHIType::Vulkan);
    // }
    
    return supportedTypes;
}

ERHIType RHIFactory::GetDefaultRHIType() {
    auto supportedTypes = GetSupportedRHITypes();
    
    // 优先选择Vulkan，回退到OpenGL
    for (auto type : {ERHIType::Vulkan, ERHIType::OpenGL}) {
        for (auto supported : supportedTypes) {
            if (type == supported) {
                return type;
            }
        }
    }
    
    return ERHIType::Unknown;
}

Bool RHIFactory::IsRHITypeSupported(ERHIType rhiType) {
    auto supportedTypes = GetSupportedRHITypes();
    for (auto supported : supportedTypes) {
        if (rhiType == supported) {
            return true;
        }
    }
    return false;
}

String RHIFactory::GetRHITypeName(ERHIType rhiType) {
    switch (rhiType) {
        case ERHIType::OpenGL: return "OpenGL";
        case ERHIType::Vulkan: return "Vulkan";
        case ERHIType::D3D11: return "Direct3D 11";
        case ERHIType::D3D12: return "Direct3D 12";
        case ERHIType::Metal: return "Metal";
        default: return "Unknown";
    }
}

// RHIContext实现
RHIContext& RHIContext::GetInstance() {
    static RHIContext instance;
    return instance;
}

Bool RHIContext::Initialize(ERHIType rhiType) {
    if (device) {
        std::cerr << "RHI已经初始化" << std::endl;
        return false;
    }

    auto devicePtr = RHIFactory::CreateDevice(rhiType);
    if (!devicePtr) {
        std::cerr << "创建RHI设备失败" << std::endl;
        return false;
    }

    device = std::shared_ptr<RHIDevice>(devicePtr.release());
    commandQueue = std::make_shared<RHICommandQueue>();

    std::cout << "RHI初始化成功: " << RHIFactory::GetRHITypeName(device->GetRHIType()) << std::endl;
    std::cout << "设备: " << device->GetDeviceName() << std::endl;
    std::cout << "厂商: " << device->GetVendorName() << std::endl;
    std::cout << "驱动版本: " << device->GetDriverVersion() << std::endl;

    return true;
}

void RHIContext::Shutdown() {
    if (commandQueue) {
        commandQueue.reset();
    }
    
    if (device) {
        device.reset();
        std::cout << "RHI已关闭" << std::endl;
    }
}

void RHIContext::SetThreadCount(U32 threadCount) {
    if (commandQueue) {
        commandQueue->SetThreadCount(threadCount);
    }
}

U32 RHIContext::GetThreadCount() const {
    if (commandQueue) {
        return commandQueue->GetThreadCount();
    }
    return 1;
}

} // namespace JzRE 