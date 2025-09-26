/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {
/**
 * @brief Render Thread Context, maintains an independent OpenGL context for each thread
 */
class JzRenderThreadContext {
public:
    /**
     * @brief Constructor
     *
     * @param threadId
     */
    JzRenderThreadContext(U32 threadId);

    /**
     * @brief Destructor
     */
    ~JzRenderThreadContext();

    /**
     * @brief Initialize the Render Thread Context
     *
     * @return Bool
     */
    Bool Initialize();

    /**
     * @brief Make the Render Thread Context Current
     */
    void MakeCurrent();

    /**
     * @brief Swap the Buffers of the Render Thread Context
     */
    void SwapBuffers();

    /**
     * @brief Cleanup the Render Thread Context
     */
    void Cleanup();

    /**
     * @brief Get the Thread Id of the Render Thread Context
     *
     * @return U32
     */
    U32 GetThreadId() const;

    /**
     * @brief Get the Thread Id of the Render Thread Context
     *
     * @return U32
     */
    Bool IsInitialized() const;

private:
    U32  threadId;
    Bool isInitialized = false;

    // OpenGL上下文相关
    // GLFWwindow* sharedContext = nullptr;
    // 具体实现依赖于窗口系统
};
} // namespace JzRE