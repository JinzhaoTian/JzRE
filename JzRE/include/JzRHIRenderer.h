#pragma once

#include "JzRHIFramebuffer.h"
#include "JzRHIPipeline.h"
#include "JzRHIStats.h"
#include "JzRHITexture.h"
#include "OGLRenderWindow.h"
#include "OGLScene.h"

namespace JzRE {

/**
 * @brief RHI Renderer, providing cross-platform rendering support
 */
class JzRHIRenderer {
public:
    JzRHIRenderer(std::shared_ptr<OGLRenderWindow> wnd, I32 width, I32 height);
    ~JzRHIRenderer();

    // 主要渲染接口
    void RenderScene(std::shared_ptr<OGLScene> scene);
    Bool Initialize();
    void Shutdown();

    // 渲染模式切换
    void SetUseCommandBuffer(Bool useCommandBuffer);
    Bool IsUsingCommandBuffer() const;

    // 多线程渲染支持
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;

    // 统计信息
    const JzRHIStats &GetStats() const;
    void              ResetStats();

private:
    // 初始化相关
    Bool CreateFramebuffer();
    Bool CreateDefaultPipeline();
    void CleanupResources();

    // 立即模式渲染
    void RenderImmediate(std::shared_ptr<OGLScene> scene);

    // 命令缓冲模式渲染
    void RenderWithCommandBuffer(std::shared_ptr<OGLScene> scene);

    // 辅助函数
    void SetupViewport();
    void ClearBuffers();

private:
    I32                              width, height;
    std::shared_ptr<OGLRenderWindow> window;

    // RHI资源
    std::shared_ptr<JzRHIFramebuffer> framebuffer;
    std::shared_ptr<JzRHITexture>     colorTexture;
    std::shared_ptr<JzRHITexture>     depthTexture;
    std::shared_ptr<JzRHIPipeline>    defaultPipeline;

    // 渲染模式
    Bool useCommandBuffer = true;
    Bool isInitialized    = false;
};

} // namespace JzRE