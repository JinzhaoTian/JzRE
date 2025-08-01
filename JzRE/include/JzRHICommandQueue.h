#pragma once

#include "CommonTypes.h"
#include "JzRHICommandBuffer.h"
#include "JzRHIETypes.h"

namespace JzRE {
/**
 * @brief RHI Command Queue, Supports multi-threaded command submission and execution
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
    U32  GetThreadCount() const;

private:
    std::vector<std::shared_ptr<JzRHICommandBuffer>> pendingCommandBuffers;
    std::mutex                                       queueMutex;
    std::atomic<U32>                                 threadCount{1};
    std::atomic<Bool>                                isExecuting{false};
};
} // namespace JzRE