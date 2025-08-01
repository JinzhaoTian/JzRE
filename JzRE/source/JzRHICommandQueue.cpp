#include "JzRHICommandQueue.h"

JzRE::JzRHICommandQueue::JzRHICommandQueue() { }

JzRE::JzRHICommandQueue::~JzRHICommandQueue()
{
    Wait();
}

std::shared_ptr<JzRE::JzRHICommandBuffer> JzRE::JzRHICommandQueue::CreateCommandBuffer(const JzRE::String &debugName)
{
    return std::make_shared<JzRE::JzRHICommandBuffer>(debugName);
}

void JzRE::JzRHICommandQueue::SubmitCommandBuffer(std::shared_ptr<JzRE::JzRHICommandBuffer> commandBuffer)
{
    if (!commandBuffer || commandBuffer->IsEmpty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(queueMutex);
    pendingCommandBuffers.push_back(commandBuffer);
}

void JzRE::JzRHICommandQueue::ExecuteAll()
{
    if (isExecuting) {
        return;
    }

    isExecuting = true;

    std::vector<std::shared_ptr<JzRHICommandBuffer>> commandBuffersToExecute;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandBuffersToExecute = pendingCommandBuffers;
        pendingCommandBuffers.clear();
    }

    // 单线程执行所有命令缓冲
    // TODO: 实现多线程并行执行
    for (auto &commandBuffer : commandBuffersToExecute) {
        commandBuffer->Execute();
    }

    isExecuting = false;
}

void JzRE::JzRHICommandQueue::Wait()
{
    // while (isExecuting) {
    //     std::this_thread::sleep_for(std::chrono::microseconds(100));
    // }
}

void JzRE::JzRHICommandQueue::SetThreadCount(JzRE::U32 threadCount)
{
    this->threadCount = std::max(1u, threadCount);
}

JzRE::U32 JzRE::JzRHICommandQueue::GetThreadCount() const
{
    return threadCount;
}