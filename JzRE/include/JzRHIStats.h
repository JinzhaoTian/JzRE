#pragma once

#include "CommonTypes.h"

namespace JzRE {
/**
 * @brief RHI Statistics
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