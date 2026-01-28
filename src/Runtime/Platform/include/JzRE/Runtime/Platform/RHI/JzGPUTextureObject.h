/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUResource.h"

namespace JzRE {

/**
 * @brief Enums of texture resource types
 */
enum class JzETextureResourceType : U8 {
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture2DArray
};

/**
 * @brief Enums of texture resource formats
 */
enum class JzETextureResourceFormat : U8 {
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
 * @brief Enums of texture resource filters
 */
enum class JzETextureResourceFilter : U8 {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};

/**
 * @brief Enums of texture resource wrap
 */
enum class JzETextureResourceWrap : U8 {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
};

/**
 * @brief GPU texture object description
 */
struct JzGPUTextureObjectDesc {
    JzETextureResourceType   type      = JzETextureResourceType::Texture2D;
    JzETextureResourceFormat format    = JzETextureResourceFormat::RGBA8;
    U32                      width     = 1;
    U32                      height    = 1;
    U32                      depth     = 1;
    U32                      mipLevels = 1;
    U32                      arraySize = 1;
    JzETextureResourceFilter minFilter = JzETextureResourceFilter::Linear;
    JzETextureResourceFilter magFilter = JzETextureResourceFilter::Linear;
    JzETextureResourceWrap   wrapS     = JzETextureResourceWrap::Repeat;
    JzETextureResourceWrap   wrapT     = JzETextureResourceWrap::Repeat;
    JzETextureResourceWrap   wrapR     = JzETextureResourceWrap::Repeat;
    const void              *data      = nullptr;
    String                   debugName;
};

/**
 * @brief Interface of GPU texture object, to store and sample image data
 */
class JzGPUTextureObject : public JzGPUResource {
public:
    /**
     * @brief Constructor
     *
     * @param desc The description of the texture
     */
    JzGPUTextureObject(const JzGPUTextureObjectDesc &desc) :
        JzGPUResource(desc.debugName), desc(desc) { }

    /**
     * @brief Destructor
     */
    virtual ~JzGPUTextureObject() = default;

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
    JzETextureResourceType GetType() const
    {
        return desc.type;
    }

    /**
     * @brief Get the format of the texture
     *
     * @return The format of the texture
     */
    JzETextureResourceFormat GetFormat() const
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
    JzGPUTextureObjectDesc desc;
};

} // namespace JzRE