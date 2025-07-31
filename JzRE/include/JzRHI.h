#pragma once

#include "CommonTypes.h"

namespace JzRE {

/**
 * RHI能力查询接口
 */
struct JzRHICapabilities {
    // 纹理支持
    U32 maxTextureSize        = 0;
    U32 maxTexture3DSize      = 0;
    U32 maxTextureArrayLayers = 0;
    U32 maxCubeMapTextureSize = 0;

    // 渲染目标支持
    U32 maxColorAttachments = 0;
    U32 maxRenderTargetSize = 0;

    // 着色器支持
    U32 maxVertexAttributes      = 0;
    U32 maxUniformBufferBindings = 0;
    U32 maxTextureUnits          = 0;

    // 几何体支持
    U32 maxVertices = 0;
    U32 maxIndices  = 0;

    // 多重采样支持
    U32 maxSamples = 0;

    // 计算着色器支持
    Bool supportsComputeShaders         = false;
    U32  maxComputeWorkGroupSize[3]     = {0, 0, 0};
    U32  maxComputeWorkGroupInvocations = 0;

    // 几何着色器支持
    Bool supportsGeometryShaders = false;

    // 细分着色器支持
    Bool supportsTessellationShaders = false;

    // 多线程支持
    Bool supportsMultithreadedRendering = false;
    U32  maxRenderThreads               = 1;
};

/**
 * RHI统计信息
 */
struct JzRHIStats {
    // 绘制调用统计
    U32 drawCalls = 0;
    U32 triangles = 0;
    U32 vertices  = 0;

    // 资源统计
    U32 buffers   = 0;
    U32 textures  = 0;
    U32 shaders   = 0;
    U32 pipelines = 0;

    // 内存统计
    Size bufferMemory  = 0;
    Size textureMemory = 0;
    Size totalMemory   = 0;

    // 性能统计
    F32 frameTime = 0.0f;
    F32 gpuTime   = 0.0f;

    void Reset();
};

} // namespace JzRE