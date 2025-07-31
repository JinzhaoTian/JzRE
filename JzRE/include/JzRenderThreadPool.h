#pragma once

#include "CommonTypes.h"
#include "JzRHITypes.h"

namespace JzRE {

/**
 * 渲染线程池
 * 管理多个渲染线程，支持任务调度和负载均衡
 */
class JzRenderThreadPool {
public:
    JzRenderThreadPool(U32 threadCount = std::thread::hardware_concurrency());
    ~JzRenderThreadPool();

    // 线程池管理
    void Start();
    void Stop();
    Bool IsRunning() const
    {
        return isRunning;
    }

    // 任务提交
    void SubmitTask(std::shared_ptr<JzRenderTask> task);
    void SubmitCommandBuffer(std::shared_ptr<JzRHICommandBuffer> commandBuffer, U32 priority = 0);
    void SubmitResourceUpdate(std::function<void()> updateFunc, U32 priority = 1);

    // 同步操作
    void WaitForCompletion();
    void Flush();

    // 线程池配置
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const
    {
        return threadCount;
    }
    U32 GetActiveThreadCount() const
    {
        return activeThreads;
    }

    // 统计信息
    Size GetPendingTaskCount() const;
    Size GetCompletedTaskCount() const
    {
        return completedTasks;
    }
    void ResetStats();

private:
    U32               threadCount;
    std::atomic<U32>  activeThreads{0};
    std::atomic<Bool> isRunning{false};
    std::atomic<Bool> shouldStop{false};

    std::vector<std::thread>                                                                                       workers;
    std::priority_queue<std::shared_ptr<JzRenderTask>, std::vector<std::shared_ptr<JzRenderTask>>, TaskComparator> tasks;

    mutable std::mutex      taskMutex;
    std::condition_variable taskCondition;
    std::condition_variable completionCondition;

    std::atomic<Size> completedTasks{0};

    void WorkerThread(U32 threadId);
};
}