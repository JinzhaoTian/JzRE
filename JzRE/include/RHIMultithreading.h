#pragma once

#include "RHICommand.h"
#include <thread>
#include <condition_variable>
#include <future>
#include <queue>
#include <functional>

namespace JzRE {

/**
 * 渲染任务类型
 */
enum class ERenderTaskType : U8 {
    CommandBuffer,
    ResourceUpdate,
    GeometryProcessing,
    ShaderCompilation
};

/**
 * 渲染任务基类
 */
class RenderTask {
public:
    RenderTask(ERenderTaskType type, U32 priority = 0) 
        : type(type), priority(priority) {}
    virtual ~RenderTask() = default;

    ERenderTaskType GetType() const { return type; }
    U32 GetPriority() const { return priority; }
    virtual void Execute() = 0;

protected:
    ERenderTaskType type;
    U32 priority;
};

/**
 * 命令缓冲任务
 */
class CommandBufferTask : public RenderTask {
public:
    CommandBufferTask(std::shared_ptr<RHICommandBuffer> commandBuffer, U32 priority = 0);
    void Execute() override;

private:
    std::shared_ptr<RHICommandBuffer> commandBuffer;
};

/**
 * 资源更新任务
 */
class ResourceUpdateTask : public RenderTask {
public:
    ResourceUpdateTask(std::function<void()> updateFunc, U32 priority = 1);
    void Execute() override;

private:
    std::function<void()> updateFunc;
};

/**
 * 任务比较器（用于优先队列）
 */
struct TaskComparator {
    bool operator()(const std::shared_ptr<RenderTask>& a, const std::shared_ptr<RenderTask>& b) const {
        return a->GetPriority() < b->GetPriority();
    }
};

/**
 * 渲染线程池
 * 管理多个渲染线程，支持任务调度和负载均衡
 */
class RenderThreadPool {
public:
    RenderThreadPool(U32 threadCount = std::thread::hardware_concurrency());
    ~RenderThreadPool();

    // 线程池管理
    void Start();
    void Stop();
    Bool IsRunning() const { return isRunning; }

    // 任务提交
    void SubmitTask(std::shared_ptr<RenderTask> task);
    void SubmitCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer, U32 priority = 0);
    void SubmitResourceUpdate(std::function<void()> updateFunc, U32 priority = 1);

    // 同步操作
    void WaitForCompletion();
    void Flush();

    // 线程池配置
    void SetThreadCount(U32 threadCount);
    U32 GetThreadCount() const { return threadCount; }
    U32 GetActiveThreadCount() const { return activeThreads; }

    // 统计信息
    Size GetPendingTaskCount() const;
    Size GetCompletedTaskCount() const { return completedTasks; }
    void ResetStats();

private:
    U32 threadCount;
    std::atomic<U32> activeThreads{0};
    std::atomic<Bool> isRunning{false};
    std::atomic<Bool> shouldStop{false};
    
    std::vector<std::thread> workers;
    std::priority_queue<std::shared_ptr<RenderTask>, std::vector<std::shared_ptr<RenderTask>>, TaskComparator> tasks;
    
    mutable std::mutex taskMutex;
    std::condition_variable taskCondition;
    std::condition_variable completionCondition;
    
    std::atomic<Size> completedTasks{0};
    
    void WorkerThread(U32 threadId);
};

/**
 * 渲染线程上下文
 * 为每个线程维护独立的OpenGL上下文
 */
class RenderThreadContext {
public:
    RenderThreadContext(U32 threadId);
    ~RenderThreadContext();

    // 上下文管理
    Bool Initialize();
    void MakeCurrent();
    void SwapBuffers();
    void Cleanup();

    U32 GetThreadId() const { return threadId; }
    Bool IsInitialized() const { return isInitialized; }

private:
    U32 threadId;
    Bool isInitialized = false;
    
    // OpenGL上下文相关
    // GLFWwindow* sharedContext = nullptr;
    // 具体实现依赖于窗口系统
};

/**
 * 多线程渲染管理器
 * 协调多线程渲染流程，管理线程间同步
 */
class MultithreadedRenderManager {
public:
    MultithreadedRenderManager();
    ~MultithreadedRenderManager();

    // 管理器生命周期
    Bool Initialize(U32 threadCount = 0);
    void Shutdown();
    Bool IsInitialized() const { return isInitialized; }

    // 渲染流程控制
    void BeginFrame();
    void EndFrame();
    void Present();

    // 命令提交
    void SubmitCommandBuffer(std::shared_ptr<RHICommandBuffer> commandBuffer);
    void SubmitRenderPass(std::function<void()> renderFunc);

    // 资源操作
    void UpdateBuffer(std::shared_ptr<RHIBuffer> buffer, const void* data, Size size);
    void UpdateTexture(std::shared_ptr<RHITexture> texture, const void* data);

    // 配置
    void SetThreadCount(U32 threadCount);
    U32 GetThreadCount() const;
    void SetVSyncEnabled(Bool enabled) { vsyncEnabled = enabled; }

    // 性能监控
    const RHIStats& GetRenderStats() const;
    F32 GetAverageFrameTime() const { return averageFrameTime; }
    F32 GetThreadUtilization() const;

private:
    Bool isInitialized = false;
    Bool vsyncEnabled = true;
    U32 frameCount = 0;
    F32 averageFrameTime = 0.0f;
    
    std::unique_ptr<RenderThreadPool> threadPool;
    std::vector<std::unique_ptr<RenderThreadContext>> threadContexts;
    
    // 同步对象
    std::mutex frameMutex;
    std::condition_variable frameComplete;
    std::atomic<Bool> frameInProgress{false};
    
    // 性能统计
    RHIStats renderStats;
    std::chrono::high_resolution_clock::time_point frameStartTime;
    
    void CreateThreadContexts(U32 threadCount);
    void DestroyThreadContexts();
    void WaitForFrameCompletion();
};

/**
 * 多线程渲染工具函数
 */
namespace MultithreadingUtils {
    // 获取推荐的线程数量
    U32 GetRecommendedThreadCount();
    
    // 检查多线程渲染支持
    Bool IsMultithreadedRenderingSupported();
    
    // 线程亲和性设置
    void SetThreadAffinity(std::thread& thread, U32 coreId);
    
    // 性能分析
    void BeginProfileRegion(const String& name);
    void EndProfileRegion();
}

} // namespace JzRE 