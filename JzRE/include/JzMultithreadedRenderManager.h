#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"

namespace JzRE {
/**
 * 多线程渲染管理器
 * 协调多线程渲染流程，管理线程间同步
 */
class JzMultithreadedRenderManager {
public:
    JzMultithreadedRenderManager();
    ~JzMultithreadedRenderManager();

    // 管理器生命周期
    Bool Initialize(U32 threadCount = 0);
    void Shutdown();
    Bool IsInitialized() const
    {
        return isInitialized;
    }

    // 渲染流程控制
    void BeginFrame();
    void EndFrame();
    void Present();

    // 命令提交
    void SubmitCommandBuffer(std::shared_ptr<JzRHICommandBuffer> commandBuffer);
    void SubmitRenderPass(std::function<void()> renderFunc);

    // 资源操作
    void UpdateBuffer(std::shared_ptr<JzRHIBuffer> buffer, const void *data, Size size);
    void UpdateTexture(std::shared_ptr<JzRHITexture> texture, const void *data);

    // 配置
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;
    void SetVSyncEnabled(Bool enabled)
    {
        vsyncEnabled = enabled;
    }

    // 性能监控
    const JzRHIStats &GetRenderStats() const;
    F32               GetAverageFrameTime() const
    {
        return averageFrameTime;
    }
    F32 GetThreadUtilization() const;

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

    void CreateThreadContexts(U32 threadCount);
    void DestroyThreadContexts();
    void WaitForFrameCompletion();
};

} // namespace JzRE