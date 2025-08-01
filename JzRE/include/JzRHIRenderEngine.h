#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIRenderer.h"

namespace JzRE {
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
    Bool InitScene();
    void ProcessInput();
    void UpdateStats();

private:
    const I32    wndWidth  = 1200;
    const I32    wndHeight = 800;
    const String title     = "JzRE - RHI Rendering Engine";

    Bool isRunning;
    Bool isInitialized;

    // 引擎组件
    // std::shared_ptr<OGLRenderWindow> window;
    std::shared_ptr<JzRHIRenderer> renderer;
    // std::shared_ptr<OGLScene>        scene;
    // std::shared_ptr<OGLCamera>       camera;

    // 性能监控
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    F32                                            frameTime  = 0.0f;
    U32                                            frameCount = 0;
};

} // namespace JzRE