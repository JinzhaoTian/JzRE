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
    /**
     * @brief Constructor
     */
    JzRHICommandQueue();

    /**
     * @brief Destructor
     */
    ~JzRHICommandQueue();

    /**
     * @brief Create a command buffer
     * @param debugName The debug name of the command buffer
     * @return The created command buffer
     */
    std::shared_ptr<JzRHICommandBuffer> CreateCommandBuffer(const String &debugName = "");

    /**
     * @brief Submit a command buffer
     * @param commandBuffer The command buffer to submit
     */
    void SubmitCommandBuffer(std::shared_ptr<JzRHICommandBuffer> commandBuffer);

    /**
     * @brief Execute all command buffers
     */
    void ExecuteAll();

    /**
     * @brief Wait for all command buffers to be executed
     */
    void Wait();

    /**
     * @brief Set the thread count
     * @param threadCount The thread count
     */
    void SetThreadCount(U32 threadCount);

    /**
     * @brief Get the thread count
     */
    U32 GetThreadCount() const;

private:
    std::vector<std::shared_ptr<JzRHICommandBuffer>> pendingCommandBuffers;
    std::mutex                                       queueMutex;
    std::atomic<U32>                                 threadCount{1};
    std::atomic<Bool>                                isExecuting{false};
};
} // namespace JzRE