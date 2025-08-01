
#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"

namespace JzRE {
/**
 * @brief Render Thread Context, maintains an independent OpenGL context for each thread
 */
class JzRenderThreadContext {
public:
    JzRenderThreadContext(U32 threadId);
    ~JzRenderThreadContext();

    // 上下文管理
    Bool Initialize();
    void MakeCurrent();
    void SwapBuffers();
    void Cleanup();

    U32  GetThreadId() const;
    Bool IsInitialized() const;

private:
    U32  threadId;
    Bool isInitialized = false;

    // OpenGL上下文相关
    // GLFWwindow* sharedContext = nullptr;
    // 具体实现依赖于窗口系统
};
} // namespace JzRE