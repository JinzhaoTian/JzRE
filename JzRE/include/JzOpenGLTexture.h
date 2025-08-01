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

    GLuint GetHandle() const;
    GLenum GetTarget() const;

private:
    static GLenum ConvertTextureType(JzETextureType type);
    static GLenum ConvertInternalFormat(JzETextureFormat format);
    static GLenum ConvertFormat(JzETextureFormat format);
    static GLenum ConvertType(JzETextureFormat format);
    static GLenum ConvertFilter(JzETextureFilter filter);
    static GLenum ConvertWrap(JzETextureWrap wrap);

private:
    GLuint handle = 0;
    GLenum target;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

} // namespace JzRE