#pragma once

#include "JzRHI.h"
#include "OGLRenderWindow.h"
#include "OGLScene.h"

namespace JzRE {

/**
 * 基于RHI的渲染器
 * 替换原有的OGLRenderer，提供跨平台渲染支持
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
    void SetUseCommandBuffer(Bool useCommandBuffer)
    {
        this->useCommandBuffer = useCommandBuffer;
    }
    Bool IsUsingCommandBuffer() const
    {
        return useCommandBuffer;
    }

    // 多线程渲染支持
    void SetThreadCount(U32 threadCount);
    U32  GetThreadCount() const;

    // 统计信息
    const JzRHIStats &GetStats() const;
    void              ResetStats();

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
};

/**
 * RHI渲染引擎
 * 替换原有的OGLRenderEngine，使用RHI接口
 */
class JzRHIRenderEngine {
public:
    JzRHIRenderEngine();
    ~JzRHIRenderEngine();

    Bool Initialize(JzERHIType rhiType = JzERHIType::Unknown);
    void Run();
    void Shutdown();

    // 设置选项
    void SetUseCommandBuffer(Bool useCommandBuffer);
    void SetThreadCount(U32 threadCount);

private:
    const I32    wndWidth  = 1200;
    const I32    wndHeight = 800;
    const String title     = "JzRE - RHI Rendering Engine";

    Bool isRunning;
    Bool isInitialized;

    // 引擎组件
    std::shared_ptr<OGLRenderWindow> window;
    std::shared_ptr<JzRHIRenderer>   renderer;
    std::shared_ptr<OGLScene>        scene;
    std::shared_ptr<OGLCamera>       camera;

    Bool InitScene();
    void ProcessInput();
    void UpdateStats();

    // 性能监控
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    F32                                            frameTime  = 0.0f;
    U32                                            frameCount = 0;
};

} // namespace JzRE