#pragma once

#include "CommonTypes.h"

namespace JzRE {
/**
 * @brief Interface of RHI Capabilities
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
} // namespace JzRE