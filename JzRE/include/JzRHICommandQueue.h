#pragma once

#include "CommonTypes.h"
#include "JzRHICommandBuffer.h"
#include "JzRHITypes.h"

namespace JzRE {

/**
 * 命令队列管理器
 * 支持多线程命令提交和执行
 */
class JzRHICommandQueue {
public:
    JzRHICommandQueue();
    ~JzRHICommandQueue();

    // 命令缓冲管理
    std::shared_ptr<JzRHICommandBuffer> CreateCommandBuffer(const String &debugName = "");
    void                                SubmitCommandBuffer(std::shared_ptr<JzRHICommandBuffer> commandBuffer);
    void                                ExecuteAll();
    void                                Wait();

    // 多线程支持
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const
    {
        return threadCount;
    }

private:
    std::vector<std::shared_ptr<JzRHICommandBuffer>> pendingCommandBuffers;
    std::mutex                                       queueMutex;
    std::atomic<U32>                                 threadCount{1};
    std::atomic<Bool>                                isExecuting{false};
};
} // namespace JzRE