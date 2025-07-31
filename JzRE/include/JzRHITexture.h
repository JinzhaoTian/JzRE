#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
/**
 * RHI纹理接口
 */
class JzRHITexture : public JzRHIResource {
public:
    JzRHITexture(const JzTextureDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHITexture() = default;

    virtual void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) = 0;
    virtual void GenerateMipmaps()                                                  = 0;

    JzETextureType GetType() const
    {
        return desc.type;
    }
    JzETextureFormat GetFormat() const
    {
        return desc.format;
    }
    U32 GetWidth() const
    {
        return desc.width;
    }
    U32 GetHeight() const
    {
        return desc.height;
    }
    U32 GetDepth() const
    {
        return desc.depth;
    }
    U32 GetMipLevels() const
    {
        return desc.mipLevels;
    }

protected:
    JzTextureDesc desc;
};

} // namespace JzRE