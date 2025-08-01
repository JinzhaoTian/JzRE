#pragma once

#include "CommonTypes.h"
#include "JzRHIBuffer.h"
#include "JzRHICommandBuffer.h"
#include "JzRHIETypes.h"
#include "JzRHIStats.h"
#include "JzRenderThreadContext.h"
#include "JzRenderThreadPool.h"

namespace JzRE {

// 获取推荐的线程数量
U32 GetRecommendedThreadCount();

// 检查多线程渲染支持
Bool IsMultithreadedRenderingSupported();

// 线程亲和性设置
void SetThreadAffinity(std::thread &thread, U32 coreId);

// 性能分析
void BeginProfileRegion(const String &name);
void EndProfileRegion();

/**
 * @brief Multithreaded Render Manager, coordinates the multithreaded rendering process, manages thread synchronization
 */
class JzMultithreadedRenderManager {
public:
    JzMultithreadedRenderManager();
    ~JzMultithreadedRenderManager();

    // 管理器生命周期
    Bool Initialize(U32 threadCount = 0);
    void Shutdown();
    Bool IsInitialized() const;

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