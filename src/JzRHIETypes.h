/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"

namespace JzRE {

/**
 * @brief Supported graphics API types
 */
enum class JzERHIType : U8 {
    Unknown = 0,
    OpenGL,
    Vulkan,
    D3D11,
    D3D12,
    Metal
};

/**
 * @brief Buffer type
 */
enum class JzEBufferType : U8 {
    Vertex,
    Index,
    Uniform,
    Storage
};

/**
 * @brief Buffer usage
 */
enum class JzEBufferUsage : U8 {
    StaticDraw,
    DynamicDraw,
    StreamDraw
};

/**
 * @brief Texture type
 */
enum class JzETextureType : U8 {
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture2DArray
};

/**
 * @brief Texture format
 */
enum class JzETextureFormat : U8 {
    Unknown,
    R8,
    RG8,
    RGB8,
    RGBA8,
    R16F,
    RG16F,
    RGB16F,
    RGBA16F,
    R32F,
    RG32F,
    RGB32F,
    RGBA32F,
    Depth16,
    Depth24,
    Depth32F,
    Depth24Stencil8
};

/**
 * @brief Texture filter
 */
enum class JzETextureFilter : U8 {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};

/**
 * @brief Texture wrap
 */
enum class JzETextureWrap : U8 {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
};

/**
 * @brief Shader type
 */
enum class JzEShaderType : U8 {
    Vertex,
    Fragment,
    Geometry,
    TessellationControl,
    TessellationEvaluation,
    Compute
};

/**
 * @brief Primitive type
 */
enum class JzEPrimitiveType : U8 {
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan
};

/**
 * @brief Blend mode
 */
enum class JzEBlendMode : U8 {
    None,
    Alpha,
    Additive,
    Multiply
};

/**
 * @brief Depth test function
 */
enum class JzEDepthFunc : U8 {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

/**
 * @brief Face culling mode
 */
enum class JzECullMode : U8 {
    None,
    Front,
    Back,
    FrontAndBack
};

/**
 * @brief MSAA level
 */
enum class JzEMSAALevel : U8 {
    None = 0,
    X2   = 2,
    X4   = 4,
    X8   = 8,
    X16  = 16
};

/**
 * @brief Render mode
 */
enum class JzERenderMode : U8 {
    Immediate,
    CommandBuffer
};

/**
 * @brief Render Task Type
 */
enum class JzERenderTaskType : U8 {
    CommandBuffer,
    ResourceUpdate,
    GeometryProcessing,
    ShaderCompilation
};

/**
 * @brief Render pass type
 */
enum class JzERenderPassType : U8 {
    BasePass,
    ShadowPass,
    PostProcess,
    UI,
    Debug
};

/**
 * @brief Viewport description
 */
struct JzViewport {
    F32 x        = 0.0f;
    F32 y        = 0.0f;
    F32 width    = 800.0f;
    F32 height   = 600.0f;
    F32 minDepth = 0.0f;
    F32 maxDepth = 1.0f;
};

/**
 * @brief Scissor rectangle description
 */
struct JzScissorRect {
    I32 x      = 0;
    I32 y      = 0;
    U32 width  = 800;
    U32 height = 600;
};

/**
 * @brief Clear parameters
 */
struct JzClearParams {
    Bool clearColor   = true;
    Bool clearDepth   = true;
    Bool clearStencil = false;
    F32  colorR       = 0.0f;
    F32  colorG       = 0.0f;
    F32  colorB       = 0.0f;
    F32  colorA       = 1.0f;
    F32  depth        = 1.0f;
    U32  stencil      = 0;
};

/**
 * @brief Draw parameters
 */
struct JzDrawParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              vertexCount   = 0;
    U32              instanceCount = 1;
    U32              firstVertex   = 0;
    U32              firstInstance = 0;
};

/**
 * @brief Draw indexed parameters
 */
struct JzDrawIndexedParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              indexCount    = 0;
    U32              instanceCount = 1;
    U32              firstIndex    = 0;
    I32              vertexOffset  = 0;
    U32              firstInstance = 0;
};

/**
 * @brief Render state description
 */
struct JzRenderState {
    JzEBlendMode blendMode  = JzEBlendMode::None;
    JzEDepthFunc depthFunc  = JzEDepthFunc::Less;
    JzECullMode  cullMode   = JzECullMode::Back;
    Bool         depthTest  = true;
    Bool         depthWrite = true;
    Bool         wireframe  = false;
    JzEMSAALevel msaaLevel  = JzEMSAALevel::None;
};

} // namespace JzRE