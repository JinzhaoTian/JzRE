#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHITexture.h"

namespace JzRE {
/**
 * OpenGL纹理实现
 */
class JzOpenGLTexture : public JzRHITexture {
public:
    JzOpenGLTexture(const JzTextureDesc &desc);
    ~JzOpenGLTexture() override;

    void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) override;
    void GenerateMipmaps() override;

    GLuint GetHandle() const
    {
        return handle;
    }
    GLenum GetTarget() const
    {
        return target;
    }

private:
    GLuint handle = 0;
    GLenum target;
    GLenum internalFormat;
    GLenum format;
    GLenum type;

    static GLenum ConvertTextureType(ETextureType type);
    static GLenum ConvertInternalFormat(ETextureFormat format);
    static GLenum ConvertFormat(ETextureFormat format);
    static GLenum ConvertType(ETextureFormat format);
    static GLenum ConvertFilter(ETextureFilter filter);
    static GLenum ConvertWrap(ETextureWrap wrap);
};

} // namespace JzRE