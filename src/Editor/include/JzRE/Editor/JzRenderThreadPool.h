/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <queue>
#include <thread>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHICommandList.h"
#include "JzRE/Editor/JzRenderTask.h"
#include "JzRE/Editor/JzTaskComparator.h"

namespace JzRE {
/**
 * @brief Render Thread Pool, manages multiple render threads, supports task scheduling and load balancing
 */
class JzRenderThreadPool {
public:
    JzRenderThreadPool(U32 threadCount = std::thread::hardware_concurrency());
    ~JzRenderThreadPool();

    // 线程池管理
    void Start();
    void Stop();
    Bool IsRunning() const;

    // 任务提交
    void SubmitTask(std::shared_ptr<JzRenderTask> task);
    void SubmitCommandList(std::shared_ptr<JzRHICommandList> commandList, U32 priority = 0);
    void SubmitResourceUpdate(std::function<void()> updateFunc, U32 priority = 1);

    // 同步操作
    void WaitForCompletion();
    void Flush();

    // 线程池配置
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;
    U32  GetActiveThreadCount() const;

    // 统计信息
    Size GetPendingTaskCount() const;
    Size GetCompletedTaskCount() const;
    void ResetStats();

private:
    void WorkerThread(U32 threadId);

private:
    U32               threadCount;
    std::atomic<U32>  activeThreads{0};
    std::atomic<Bool> isRunning{false};
    std::atomic<Bool> shouldStop{false};

    std::vector<std::thread>                                                                                         workers;
    std::priority_queue<std::shared_ptr<JzRenderTask>, std::vector<std::shared_ptr<JzRenderTask>>, JzTaskComparator> tasks;

    mutable std::mutex      taskMutex;
    std::condition_variable taskCondition;
    std::condition_variable completionCondition;

    std::atomic<Size> completedTasks{0};
};
} // namespace JzRE