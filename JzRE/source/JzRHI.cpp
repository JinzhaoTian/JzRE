#include "JzRHI.h"
#include "JzOpenGLRHI.h"
#include "JzVulkanRHI.h"

namespace JzRE {

// RHIStats实现
void JzRHIStats::Reset()
{
    drawCalls     = 0;
    triangles     = 0;
    vertices      = 0;
    buffers       = 0;
    textures      = 0;
    shaders       = 0;
    pipelines     = 0;
    bufferMemory  = 0;
    textureMemory = 0;
    totalMemory   = 0;
    frameTime     = 0.0f;
    gpuTime       = 0.0f;
}

// RHIFactory实现
std::unique_ptr<JzRHIDevice> JzRHIFactory::CreateDevice(JzERHIType rhiType)
{
    if (rhiType == JzERHIType::Unknown) {
        rhiType = GetDefaultRHIType();
    }

    switch (rhiType) {
        case JzERHIType::OpenGL:
            return std::make_unique<JzOpenGLDevice>();
        case JzERHIType::Vulkan:
            return std::make_unique<JzVulkanDevice>();
        default:
            std::cerr << "不支持的RHI类型: " << static_cast<int>(rhiType) << std::endl;
            return nullptr;
    }
}

std::vector<JzERHIType> JzRHIFactory::GetSupportedRHITypes()
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

JzERHIType JzRHIFactory::GetDefaultRHIType()
{
    auto supportedTypes = GetSupportedRHITypes();

    // 优先选择Vulkan，回退到OpenGL
    for (auto type : {JzERHIType::Vulkan, JzERHIType::OpenGL}) {
        for (auto supported : supportedTypes) {
            if (type == supported) {
                return type;
            }
        }
    }

    return JzERHIType::Unknown;
}

Bool JzRHIFactory::IsRHITypeSupported(JzERHIType rhiType)
{
    auto supportedTypes = GetSupportedRHITypes();
    for (auto supported : supportedTypes) {
        if (rhiType == supported) {
            return true;
        }
    }
    return false;
}

String JzRHIFactory::GetRHITypeName(JzERHIType rhiType)
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

// RHIContext实现
JzRHIContext &JzRHIContext::GetInstance()
{
    static JzRHIContext instance;
    return instance;
}

Bool JzRHIContext::Initialize(JzERHIType rhiType)
{
    if (device) {
        std::cerr << "RHI已经初始化" << std::endl;
        return false;
    }

    auto devicePtr = JzRHIFactory::CreateDevice(rhiType);
    if (!devicePtr) {
        std::cerr << "创建RHI设备失败" << std::endl;
        return false;
    }

    device       = std::shared_ptr<JzRHIDevice>(devicePtr.release());
    commandQueue = std::make_shared<JzRHICommandQueue>();

    std::cout << "RHI初始化成功: " << JzRHIFactory::GetRHITypeName(device->GetRHIType()) << std::endl;
    std::cout << "设备: " << device->GetDeviceName() << std::endl;
    std::cout << "厂商: " << device->GetVendorName() << std::endl;
    std::cout << "驱动版本: " << device->GetDriverVersion() << std::endl;

    return true;
}

void JzRHIContext::Shutdown()
{
    if (commandQueue) {
        commandQueue.reset();
    }

    if (device) {
        device.reset();
        std::cout << "RHI已关闭" << std::endl;
    }
}

void JzRHIContext::SetThreadCount(U32 threadCount)
{
    if (commandQueue) {
        commandQueue->SetThreadCount(threadCount);
    }
}

U32 JzRHIContext::GetThreadCount() const
{
    if (commandQueue) {
        return commandQueue->GetThreadCount();
    }
    return 1;
}

} // namespace JzRE