/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"

namespace JzRE {
/**
 * @brief Interface of RHI Capabilities
 */
struct JzRHICapabilities {
    U32  maxTextureSize                 = 0;         // Texture Support
    U32  maxTexture3DSize               = 0;         //
    U32  maxTextureArrayLayers          = 0;         //
    U32  maxCubeMapTextureSize          = 0;         //
    U32  maxColorAttachments            = 0;         // Render Target Support
    U32  maxRenderTargetSize            = 0;         //
    U32  maxVertexAttributes            = 0;         // Vertex Attribute Support
    U32  maxUniformBufferBindings       = 0;         //
    U32  maxTextureUnits                = 0;         //
    U32  maxVertices                    = 0;         // Geometry Support
    U32  maxIndices                     = 0;         //
    U32  maxSamples                     = 0;         // Multisample Support
    Bool supportsComputeShaders         = false;     // Compute Shader Support
    U32  maxComputeWorkGroupSize[3]     = {0, 0, 0}; //
    U32  maxComputeWorkGroupInvocations = 0;         //
    Bool supportsGeometryShaders        = false;     // Geometry Shader Support
    Bool supportsTessellationShaders    = false;     // Tessellation Shader Support
    Bool supportsMultithreadedRendering = false;     // Multithreaded Rendering Support
    U32  maxRenderThreads               = 1;         //
};
} // namespace JzRE