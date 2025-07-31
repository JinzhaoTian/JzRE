#include "JzRHI.h"

// RHIStats实现
void JzRE::JzRHIStats::Reset()
{
    drawCalls     = 0;
    triangles     = 0;
    vertices      = 0;
    buffers       = 0;
    textures      = 0;
    shaders       = 0;
    pipelines     = 0;
    bufferMemory  = 0;
    textureMemory = 0;
    totalMemory   = 0;
    frameTime     = 0.0f;
    gpuTime       = 0.0f;
}