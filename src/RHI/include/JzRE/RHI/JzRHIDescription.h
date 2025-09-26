/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHIETypes.h"

namespace JzRE {
/**
 * @brief Buffer description
 */
struct JzBufferDesc {
    JzEBufferType  type;
    JzEBufferUsage usage;
    Size           size;
    const void    *data = nullptr;
    String         debugName;
};

/**
 * @brief Texture description
 */
struct JzTextureDesc {
    JzETextureType   type      = JzETextureType::Texture2D;
    JzETextureFormat format    = JzETextureFormat::RGBA8;
    U32              width     = 1;
    U32              height    = 1;
    U32              depth     = 1;
    U32              mipLevels = 1;
    U32              arraySize = 1;
    JzETextureFilter minFilter = JzETextureFilter::Linear;
    JzETextureFilter magFilter = JzETextureFilter::Linear;
    JzETextureWrap   wrapS     = JzETextureWrap::Repeat;
    JzETextureWrap   wrapT     = JzETextureWrap::Repeat;
    JzETextureWrap   wrapR     = JzETextureWrap::Repeat;
    const void      *data      = nullptr;
    String           debugName;
};

/**
 * @brief Shader description
 */
struct JzShaderDesc {
    JzEShaderType type;
    String        source;
    String        entryPoint = "main";
    String        debugName;
};

/**
 * @brief Pipeline description
 */
struct JzPipelineDesc {
    std::vector<JzShaderDesc> shaders;
    JzRenderState             renderState;
    String                    debugName;
};
} // namespace JzRE