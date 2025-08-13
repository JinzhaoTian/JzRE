#pragma once

#include "CommonTypes.h"
#include "JzRHIBuffer.h"
#include "JzRHICommandList.h"
#include "JzRHIETypes.h"
#include "JzRHIStats.h"
#include "JzRenderThreadContext.h"
#include "JzRenderThreadPool.h"

namespace JzRE {
/**
 * @brief RHI Render Command Manager, coordinates the multithreaded rendering process, manages thread synchronization
 */
class JzRHICommandManager {
public:
    JzRHICommandManager();
    ~JzRHICommandManager();

    // 管理器生命周期
    Bool Initialize(U32 threadCount = 0);
    void Shutdown();
    Bool IsInitialized() const;

    // 渲染流程控制
    void BeginFrame();
    void EndFrame();
    void Present();

    // 命令提交
    void SubmitCommandList(std::shared_ptr<JzRHICommandList> commandList);
    void SubmitRenderPass(std::function<void()> renderFunc);

    // 资源操作
    void UpdateBuffer(std::shared_ptr<JzRHIBuffer> buffer, const void *data, Size size);
    void UpdateTexture(std::shared_ptr<JzRHITexture> texture, const void *data);

    // 配置
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;
    void SetVSyncEnabled(Bool enabled);

    // 性能监控
    const JzRHIStats &GetRenderStats() const;
    F32               GetAverageFrameTime() const;
    F32               GetThreadUtilization() const;

private:
    void CreateThreadContexts(U32 threadCount);
    void DestroyThreadContexts();
    void WaitForFrameCompletion();

private:
    Bool isInitialized    = false;
    Bool vsyncEnabled     = true;
    U32  frameCount       = 0;
    F32  averageFrameTime = 0.0f;

    std::unique_ptr<JzRenderThreadPool>                 threadPool;
    std::vector<std::unique_ptr<JzRenderThreadContext>> threadContexts;

    // 同步对象
    std::mutex              frameMutex;
    std::condition_variable frameComplete;
    std::atomic<Bool>       frameInProgress{false};

    // 性能统计
    JzRHIStats                                     renderStats;
    std::chrono::high_resolution_clock::time_point frameStartTime;
};

} // namespace JzRE