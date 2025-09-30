/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHIResource.h"

namespace JzRE {

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
 * @brief Interface of RHI Texture
 */
class JzRHITexture : public JzRHIResource {
public:
    /**
     * @brief Constructor
     *
     * @param desc The description of the texture
     */
    JzRHITexture(const JzTextureDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHITexture() = default;

    /**
     * @brief Update the data
     *
     * @param data The data to update
     * @param mipLevel The mip level to update
     * @param arrayIndex The array index to update
     */
    virtual void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) = 0;

    /**
     * @brief Generate mipmaps
     */
    virtual void GenerateMipmaps() = 0;

    /**
     * @brief Get the native texture ID for rendering
     *
     * @return The native texture ID
     */
    virtual void *GetTextureID() const = 0;

    /**
     * @brief Get the type of the texture
     *
     * @return The type of the texture
     */
    JzETextureType GetType() const
    {
        return desc.type;
    }

    /**
     * @brief Get the format of the texture
     *
     * @return The format of the texture
     */
    JzETextureFormat GetFormat() const
    {
        return desc.format;
    }

    /**
     * @brief Get the width of the texture
     *
     * @return The width of the texture
     */
    U32 GetWidth() const
    {
        return desc.width;
    }

    /**
     * @brief Get the height of the texture
     *
     * @return The height of the texture
     */
    U32 GetHeight() const
    {
        return desc.height;
    }

    /**
     * @brief Get the depth of the texture
     *
     * @return The depth of the texture
     */
    U32 GetDepth() const
    {
        return desc.depth;
    }

    /**
     * @brief Get the mip levels of the texture
     *
     * @return The mip levels of the texture
     */
    U32 GetMipLevels() const
    {
        return desc.mipLevels;
    }

protected:
    JzTextureDesc desc;
};

} // namespace JzRE