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

JzRE::Bool JzRE::JzRHIContext::SwitchRHI(JzRE::JzERHIType newRHIType)
{
    std::lock_guard<std::mutex> lock(switchMutex);
    
    if (isSwitching) {
        std::cerr << "RHI切换正在进行中，请等待" << std::endl;
        return false;
    }
    
    if (!ValidateSwitchPreconditions(newRHIType)) {
        return false;
    }
    
    isSwitching = true;
    JzERHIType oldRHIType = GetRHIType();
    
    std::cout << "开始RHI切换: " << JzRHIFactory::GetRHITypeName(oldRHIType) 
              << " -> " << JzRHIFactory::GetRHITypeName(newRHIType) << std::endl;
    
    // 1. 保存当前状态
    PreserveCurrentState();
    
    // 2. 执行切换
    Bool success = InternalSwitchRHI(newRHIType);
    
    if (success) {
        // 3. 恢复状态到新设备
        RestoreStateAfterSwitch();
        
        // 4. 通知回调
        NotifySwitchCallbacks(oldRHIType, newRHIType);
        
        std::cout << "RHI切换成功" << std::endl;
    } else {
        std::cerr << "RHI切换失败，尝试回退到: " << JzRHIFactory::GetRHITypeName(fallbackRHIType) << std::endl;
        
        // 尝试回退到备用RHI
        if (fallbackRHIType != oldRHIType && CanSwitchRHI(fallbackRHIType)) {
            InternalSwitchRHI(fallbackRHIType);
        }
    }
    
    isSwitching = false;
    return success;
}

JzRE::Bool JzRE::JzRHIContext::CanSwitchRHI(JzRE::JzERHIType rhiType) const
{
    if (isSwitching) {
        return false;
    }
    
    return JzRHIFactory::IsRHITypeSupported(rhiType);
}

JzRE::Bool JzRE::JzRHIContext::IsSwitchingRHI() const
{
    return isSwitching;
}

void JzRE::JzRHIContext::PreserveCurrentState()
{
    if (!device) {
        return;
    }
    
    stateSnapshot = std::make_unique<JzRHIStateSnapshot>();
    
    // 注意：这里需要实际从设备获取当前状态
    // 示例实现 - 实际需要根据具体设备API实现
    std::cout << "保存当前RHI状态..." << std::endl;
    
    // 调用注册的迁移回调来保存资源
    for (const auto& [resourceType, callback] : migrationCallbacks) {
        callback(*stateSnapshot, device);
    }
}

void JzRE::JzRHIContext::RestoreStateAfterSwitch()
{
    if (!stateSnapshot || !device) {
        return;
    }
    
    std::cout << "恢复RHI状态到新设备..." << std::endl;
    
    // 迁移资源到新设备
    MigrateResources(*stateSnapshot, device);
    
    // 恢复渲染状态
    device->SetRenderState(stateSnapshot->renderState);
    device->SetViewport(stateSnapshot->viewport);
    device->SetScissor(stateSnapshot->scissor);
}

const JzRE::JzRHIStateSnapshot* JzRE::JzRHIContext::GetStateSnapshot() const
{
    return stateSnapshot.get();
}

void JzRE::JzRHIContext::RegisterSwitchCallback(const JzRE::String& name, RHISwitchCallback callback)
{
    switchCallbacks[name] = std::move(callback);
}

void JzRE::JzRHIContext::UnregisterSwitchCallback(const JzRE::String& name)
{
    switchCallbacks.erase(name);
}

void JzRE::JzRHIContext::RegisterMigrationCallback(const JzRE::String& resourceType, RHIMigrationCallback callback)
{
    migrationCallbacks[resourceType] = std::move(callback);
}

void JzRE::JzRHIContext::UnregisterMigrationCallback(const JzRE::String& resourceType)
{
    migrationCallbacks.erase(resourceType);
}

JzRE::Bool JzRE::JzRHIContext::MigrateResources(const JzRHIStateSnapshot& snapshot, std::shared_ptr<JzRHIDevice> newDevice)
{
    std::cout << "开始资源迁移..." << std::endl;
    
    try {
        // 迁移缓冲区
        for (const auto& bufferSnapshot : snapshot.buffers) {
            auto newBuffer = newDevice->CreateBuffer(bufferSnapshot.bufferDesc);
            if (newBuffer && !bufferSnapshot.data.empty()) {
                newBuffer->UpdateData(bufferSnapshot.data.data(), bufferSnapshot.data.size());
            }
        }
        
        // 迁移纹理
        for (const auto& textureSnapshot : snapshot.textures) {
            auto textureDesc = textureSnapshot.textureDesc;
            textureDesc.data = textureSnapshot.data.empty() ? nullptr : textureSnapshot.data.data();
            auto newTexture = newDevice->CreateTexture(textureDesc);
        }
        
        // 迁移着色器
        for (const auto& shaderSnapshot : snapshot.shaders) {
            auto newShader = newDevice->CreateShader(shaderSnapshot.shaderDesc);
        }
        
        std::cout << "资源迁移完成" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "资源迁移失败: " << e.what() << std::endl;
        return false;
    }
}

void JzRE::JzRHIContext::SetFallbackRHI(JzRE::JzERHIType fallbackType)
{
    fallbackRHIType = fallbackType;
}

JzRE::JzERHIType JzRE::JzRHIContext::GetFallbackRHI() const
{
    return fallbackRHIType;
}

JzRE::Bool JzRE::JzRHIContext::InternalSwitchRHI(JzRE::JzERHIType newRHIType)
{
    try {
        // 1. 创建新设备
        auto newDevicePtr = JzRHIFactory::CreateDevice(newRHIType);
        if (!newDevicePtr) {
            std::cerr << "创建新RHI设备失败" << std::endl;
            return false;
        }
        
        // 2. 清理旧设备
        CleanupOldDevice();
        
        // 3. 设置新设备
        device = std::shared_ptr<JzRHIDevice>(newDevicePtr.release());
        commandQueue = std::make_shared<JzRHICommandQueue>();
        
        std::cout << "新RHI设备创建成功: " << device->GetDeviceName() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "RHI切换过程中发生异常: " << e.what() << std::endl;
        return false;
    }
}

void JzRE::JzRHIContext::CleanupOldDevice()
{
    if (commandQueue) {
        commandQueue.reset();
    }
    
    if (device) {
        device.reset();
        std::cout << "旧RHI设备已清理" << std::endl;
    }
}

JzRE::Bool JzRE::JzRHIContext::ValidateSwitchPreconditions(JzRE::JzERHIType newRHIType)
{
    if (!IsInitialized()) {
        std::cerr << "RHI未初始化，无法切换" << std::endl;
        return false;
    }
    
    if (GetRHIType() == newRHIType) {
        std::cout << "目标RHI与当前RHI相同，无需切换" << std::endl;
        return false;
    }
    
    if (!CanSwitchRHI(newRHIType)) {
        std::cerr << "目标RHI不受支持或当前无法切换" << std::endl;
        return false;
    }
    
    return true;
}

void JzRE::JzRHIContext::NotifySwitchCallbacks(JzRE::JzERHIType oldType, JzRE::JzERHIType newType)
{
    for (const auto& [name, callback] : switchCallbacks) {
        try {
            callback(oldType, newType);
        } catch (const std::exception& e) {
            std::cerr << "切换回调异常 [" << name << "]: " << e.what() << std::endl;
        }
    }
}
