#pragma once

#include "CommonTypes.h"

namespace JzRE {

/**
 * 支持的图形API类型
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
 * 缓冲区类型
 */
enum class JzEBufferType : U8 {
    Vertex,
    Index,
    Uniform,
    Storage
};

/**
 * 缓冲区使用方式
 */
enum class JzEBufferUsage : U8 {
    StaticDraw,
    DynamicDraw,
    StreamDraw
};

/**
 * 纹理类型
 */
enum class JzETextureType : U8 {
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture2DArray
};

/**
 * 纹理格式
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
 * 纹理过滤方式
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
 * 纹理包装方式
 */
enum class JzETextureWrap : U8 {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
};

/**
 * 着色器类型
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
 * 图元类型
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
 * 混合模式
 */
enum class JzEBlendMode : U8 {
    None,
    Alpha,
    Additive,
    Multiply
};

/**
 * 深度测试函数
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
 * 面剔除模式
 */
enum class JzECullMode : U8 {
    None,
    Front,
    Back,
    FrontAndBack
};

/**
 * 多重采样抗锯齿级别
 */
enum class JzEMSAALevel : U8 {
    None = 0,
    X2   = 2,
    X4   = 4,
    X8   = 8,
    X16  = 16
};

/**
 * 渲染状态描述
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

/**
 * 缓冲区描述
 */
struct BufferDesc {
    JzEBufferType  type;
    JzEBufferUsage usage;
    Size           size;
    const void    *data = nullptr;
    String         debugName;
};

/**
 * 纹理描述
 */
struct TextureDesc {
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
 * 着色器描述
 */
struct ShaderDesc {
    EShaderType type;
    String      source;
    String      entryPoint = "main";
    String      debugName;
};

/**
 * 渲染管线描述
 */
struct PipelineDesc {
    std::vector<ShaderDesc> shaders;
    RenderState             renderState;
    String                  debugName;
};

} // namespace JzRE