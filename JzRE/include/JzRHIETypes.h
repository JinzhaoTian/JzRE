#pragma once

#include "CommonTypes.h"

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