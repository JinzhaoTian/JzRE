#pragma once

#include "CommonTypes.h"
#include "JzRHICommandQueue.h"
#include "JzRHIDevice.h"
#include "JzRHIETypes.h"

namespace JzRE {

/**
 * @brief RHI Macro Definitions
 */
#define RHI_DEVICE() JzRHIContext::GetInstance().GetDevice()
#define RHI_COMMAND_QUEUE() JzRHIContext::GetInstance().GetCommandQueue()
#define RHI_TYPE() JzRHIContext::GetInstance().GetRHIType()

/**
 * @brief RHI State Snapshot
 */
struct JzRHIStateSnapshot {
    JzRenderState renderState;
    JzViewport viewport;
    JzScissorRect scissor;
    
    struct ResourceSnapshot {
        JzBufferDesc bufferDesc;
        std::vector<U8> data;
        String debugName;
        void* platformHandle = nullptr;
    };
    
    struct TextureSnapshot {
        JzTextureDesc textureDesc;
        std::vector<U8> data;
        String debugName;
        void* platformHandle = nullptr;
    };
    
    struct ShaderSnapshot {
        JzShaderDesc shaderDesc;
        String debugName;
        void* platformHandle = nullptr;
    };
    
    std::vector<ResourceSnapshot> buffers;
    std::vector<TextureSnapshot> textures;
    std::vector<ShaderSnapshot> shaders;
    std::unordered_map<String, std::any> customData;
};

/**
 * @brief RHI Switch Callback
 */
using RHISwitchCallback = std::function<void(JzERHIType oldType, JzERHIType newType)>;
using RHIMigrationCallback = std::function<Bool(const JzRHIStateSnapshot&, std::shared_ptr<JzRHIDevice>)>;

/**
 * @brief RHI Context
 */
class JzRHIContext {
public:
    static JzRHIContext &GetInstance();

    // 设备管理
    Bool Initialize(JzERHIType rhiType = JzERHIType::Unknown);
    void Shutdown();
    Bool IsInitialized() const;

    // 获取当前设备
    std::shared_ptr<JzRHIDevice> GetDevice() const;
    JzERHIType                   GetRHIType() const;

    // 命令队列管理
    std::shared_ptr<JzRHICommandQueue> GetCommandQueue() const;

    // 多线程支持
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;

    // 新增：运行时RHI切换
    Bool SwitchRHI(JzERHIType newRHIType);
    Bool CanSwitchRHI(JzERHIType rhiType) const;
    Bool IsSwitchingRHI() const;
    
    // 新增：状态管理
    void PreserveCurrentState();
    void RestoreStateAfterSwitch();
    const JzRHIStateSnapshot* GetStateSnapshot() const;
    
    // 新增：回调管理
    void RegisterSwitchCallback(const String& name, RHISwitchCallback callback);
    void UnregisterSwitchCallback(const String& name);
    void RegisterMigrationCallback(const String& resourceType, RHIMigrationCallback callback);
    void UnregisterMigrationCallback(const String& resourceType);
    
    // 新增：资源迁移
    Bool MigrateResources(const JzRHIStateSnapshot& snapshot, std::shared_ptr<JzRHIDevice> newDevice);
    
    // 新增：错误处理
    void SetFallbackRHI(JzERHIType fallbackType);
    JzERHIType GetFallbackRHI() const;

private:
    JzRHIContext()                                = default;
    ~JzRHIContext()                               = default;
    JzRHIContext(const JzRHIContext &)            = delete;
    JzRHIContext &operator=(const JzRHIContext &) = delete;

    // 新增：内部切换逻辑
    Bool InternalSwitchRHI(JzERHIType newRHIType);
    void CleanupOldDevice();
    Bool ValidateSwitchPreconditions(JzERHIType newRHIType);
    void NotifySwitchCallbacks(JzERHIType oldType, JzERHIType newType);

private:
    std::shared_ptr<JzRHIDevice>       device;
    std::shared_ptr<JzRHICommandQueue> commandQueue;
    
    // 新增：切换相关状态
    Bool isSwitching = false;
    JzERHIType fallbackRHIType = JzERHIType::OpenGL;
    std::unique_ptr<JzRHIStateSnapshot> stateSnapshot;
    
    // 新增：回调管理
    std::unordered_map<String, RHISwitchCallback> switchCallbacks;
    std::unordered_map<String, RHIMigrationCallback> migrationCallbacks;
    
    // 新增：线程安全
    mutable std::mutex switchMutex;
};

} // namespace JzRE