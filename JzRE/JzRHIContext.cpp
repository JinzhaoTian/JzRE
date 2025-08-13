#include "JzRHIContext.h"
#include "JzRHIFactory.h"

// RHIContext实现
JzRE::JzRHIContext &JzRE::JzRHIContext::GetInstance()
{
    static JzRHIContext instance;
    return instance;
}

JzRE::Bool JzRE::JzRHIContext::Initialize(JzRE::JzERHIType rhiType)
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

void JzRE::JzRHIContext::Shutdown()
{
    if (commandQueue) {
        commandQueue.reset();
    }

    if (device) {
        device.reset();
        std::cout << "RHI已关闭" << std::endl;
    }
}

JzRE::Bool JzRE::JzRHIContext::IsInitialized() const
{
    return device != nullptr;
}

std::shared_ptr<JzRE::JzRHIDevice> JzRE::JzRHIContext::GetDevice() const
{
    return device;
}

JzRE::JzERHIType JzRE::JzRHIContext::GetRHIType() const
{
    return device ? device->GetRHIType() : JzERHIType::Unknown;
}

std::shared_ptr<JzRE::JzRHICommandQueue> JzRE::JzRHIContext::GetCommandQueue() const
{
    return commandQueue;
}

void JzRE::JzRHIContext::SetThreadCount(JzRE::U32 threadCount)
{
    if (commandQueue) {
        commandQueue->SetThreadCount(threadCount);
    }
}

JzRE::U32 JzRE::JzRHIContext::GetThreadCount() const
{
    if (commandQueue) {
        return commandQueue->GetThreadCount();
    }
    return 1;
}
