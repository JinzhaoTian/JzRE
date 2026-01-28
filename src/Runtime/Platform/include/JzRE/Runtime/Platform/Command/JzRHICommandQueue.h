/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"

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
     * @brief Create a command list
     *
     * @param debugName The debug name of the command list
     * @return The created command list
     */
    std::shared_ptr<JzRHICommandList> CreateCommandList(const String &debugName = "");

    /**
     * @brief Submit a command list
     *
     * @param commandList The command list to submit
     */
    void SubmitCommandList(std::shared_ptr<JzRHICommandList> commandList);

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
     *
     * @param threadCount The thread count
     */
    void SetThreadCount(U32 threadCount);

    /**
     * @brief Get the thread count
     */
    U32 GetThreadCount() const;

private:
    std::vector<std::shared_ptr<JzRHICommandList>> pendingCommandLists;
    std::mutex                                     queueMutex;
    std::atomic<U32>                               threadCount{1};
    std::atomic<Bool>                              isExecuting{false};
};
} // namespace JzRE