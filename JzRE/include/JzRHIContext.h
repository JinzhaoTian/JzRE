#pragma once

#include "CommonTypes.h"
#include "JzRHIDevice.h"
#include "JzRHITypes.h"

namespace JzRE {

/**
 * RHI便利宏定义
 */
#define JZ_RHI_DEVICE() JzRHIContext::GetInstance().GetDevice()
#define JZ_RHI_COMMAND_QUEUE() JzRHIContext::GetInstance().GetCommandQueue()
#define JZ_RHI_TYPE() JzRHIContext::GetInstance().GetRHIType()

/**
 * RHI上下文管理器
 * 管理RHI设备的生命周期和全局状态
 */
class JzRHIContext {
public:
    static JzRHIContext &GetInstance();

    // 设备管理
    Bool Initialize(JzERHIType rhiType = JzERHIType::Unknown);
    void Shutdown();
    Bool IsInitialized() const
    {
        return device != nullptr;
    }

    // 获取当前设备
    std::shared_ptr<JzRHIDevice> GetDevice() const
    {
        return device;
    }
    JzERHIType GetRHIType() const
    {
        return device ? device->GetRHIType() : JzERHIType::Unknown;
    }

    // 命令队列管理
    std::shared_ptr<JzRHICommandQueue> GetCommandQueue() const
    {
        return commandQueue;
    }

    // 多线程支持
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;

private:
    JzRHIContext()                                = default;
    ~JzRHIContext()                               = default;
    JzRHIContext(const JzRHIContext &)            = delete;
    JzRHIContext &operator=(const JzRHIContext &) = delete;

    std::shared_ptr<JzRHIDevice>       device;
    std::shared_ptr<JzRHICommandQueue> commandQueue;
};

} // namespace JzRE