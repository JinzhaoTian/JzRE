#include "JzOpenGLTexture.h"

JzRE::JzOpenGLTexture::JzOpenGLTexture(const JzRE::JzTextureDesc &desc) :
    JzRE::JzRHITexture(desc) { }

JzRE::JzOpenGLTexture::~JzOpenGLTexture() { }

void JzRE::JzOpenGLTexture::UpdateData(const void *data, JzRE::U32 mipLevel, U32 arrayIndex) { }

void JzRE::JzOpenGLTexture::GenerateMipmaps() { }

GLuint JzRE::JzOpenGLTexture::GetHandle() const
{
    return handle;
}

GLenum JzRE::JzOpenGLTexture::GetTarget() const
{
    return target;
}

GLenum JzRE::JzOpenGLTexture::ConvertTextureType(JzRE::JzETextureType type) { }

GLenum JzRE::JzOpenGLTexture::ConvertInternalFormat(JzRE::JzETextureFormat format) { }

GLenum JzRE::JzOpenGLTexture::ConvertFormat(JzRE::JzETextureFormat format) { }

GLenum JzRE::JzOpenGLTexture::ConvertType(JzRE::JzETextureFormat format) { }

GLenum JzRE::JzOpenGLTexture::ConvertFilter(JzRE::JzETextureFilter filter) { }

GLenum JzRE::JzOpenGLTexture::ConvertWrap(JzRE::JzETextureWrap wrap) { }