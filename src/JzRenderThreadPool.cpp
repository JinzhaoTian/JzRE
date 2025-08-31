#include "JzRenderThreadPool.h"

JzRE::JzRenderThreadPool::JzRenderThreadPool(JzRE::U32 threadCount) { }

JzRE::JzRenderThreadPool::~JzRenderThreadPool() { }

void JzRE::JzRenderThreadPool::Start() { }

void JzRE::JzRenderThreadPool::Stop() { }

JzRE::Bool JzRE::JzRenderThreadPool::IsRunning() const
{
    return isRunning;
}

void JzRE::JzRenderThreadPool::SubmitTask(std::shared_ptr<JzRE::JzRenderTask> task) { }

void JzRE::JzRenderThreadPool::SubmitCommandList(std::shared_ptr<JzRE::JzRHICommandList> commandList, U32 priority) { }

void JzRE::JzRenderThreadPool::SubmitResourceUpdate(std::function<void()> updateFunc, U32 priority) { }

void JzRE::JzRenderThreadPool::WaitForCompletion() { }

void JzRE::JzRenderThreadPool::Flush() { }

void JzRE::JzRenderThreadPool::SetThreadCount(JzRE::U32 threadCount) { }

JzRE::U32 JzRE::JzRenderThreadPool::GetThreadCount() const
{
    return threadCount;
}

JzRE::U32 JzRE::JzRenderThreadPool::GetActiveThreadCount() const
{
    return activeThreads;
}

JzRE::Size JzRE::JzRenderThreadPool::GetPendingTaskCount() const
{
    return 0;
}

JzRE::Size JzRE::JzRenderThreadPool::GetCompletedTaskCount() const
{
    return completedTasks;
}

void JzRE::JzRenderThreadPool::ResetStats() { }

void JzRE::JzRenderThreadPool::WorkerThread(JzRE::U32 threadId) { }