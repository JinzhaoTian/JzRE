/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/Command/JzRHICommandQueue.h"

#include <thread>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

JzRE::JzRHICommandQueue::JzRHICommandQueue() { }

JzRE::JzRHICommandQueue::~JzRHICommandQueue()
{
    Wait();
}

std::shared_ptr<JzRE::JzRHICommandList> JzRE::JzRHICommandQueue::CreateCommandList(const JzRE::String &debugName)
{
    return std::make_shared<JzRE::JzRHICommandList>(debugName);
}

void JzRE::JzRHICommandQueue::SubmitCommandList(std::shared_ptr<JzRE::JzRHICommandList> commandList)
{
    if (!commandList || commandList->IsEmpty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(queueMutex);
    pendingCommandLists.push_back(commandList);
}

void JzRE::JzRHICommandQueue::ExecuteAll()
{
    if (isExecuting) {
        return;
    }

    isExecuting = true;

    std::vector<std::shared_ptr<JzRHICommandList>> commandBuffersToExecute;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandBuffersToExecute = pendingCommandLists;
        pendingCommandLists.clear();
    }

    if (commandBuffersToExecute.empty()) {
        isExecuting = false;
        return;
    }

    if (!JzServiceContainer::Has<JzDevice>()) {
        isExecuting = false;
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    device.ExecuteCommandLists(commandBuffersToExecute);

    isExecuting = false;
}

void JzRE::JzRHICommandQueue::Wait()
{
    while (isExecuting) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void JzRE::JzRHICommandQueue::SetThreadCount(JzRE::U32 threadCount)
{
    this->threadCount = std::max(1u, threadCount);
}

JzRE::U32 JzRE::JzRHICommandQueue::GetThreadCount() const
{
    return threadCount;
}
