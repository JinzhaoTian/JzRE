#pragma once

#include "CommonTypes.h"
#include "JzRHIDesc.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
/**
 * @brief Interface of RHI Texture
 */
class JzRHITexture : public JzRHIResource {
public:
    /**
     * @brief Constructor
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
     * @brief Get the type of the texture
     * @return The type of the texture
     */
    JzETextureType GetType() const
    {
        return desc.type;
    }

    /**
     * @brief Get the format of the texture
     * @return The format of the texture
     */
    JzETextureFormat GetFormat() const
    {
        return desc.format;
    }

    /**
     * @brief Get the width of the texture
     * @return The width of the texture
     */
    U32 GetWidth() const
    {
        return desc.width;
    }

    /**
     * @brief Get the height of the texture
     * @return The height of the texture
     */
    U32 GetHeight() const
    {
        return desc.height;
    }

    /**
     * @brief Get the depth of the texture
     * @return The depth of the texture
     */
    U32 GetDepth() const
    {
        return desc.depth;
    }

    /**
     * @brief Get the mip levels of the texture
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