/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/OpenGL/JzOpenGLTexture.h"

JzRE::JzOpenGLTexture::JzOpenGLTexture(const JzRE::JzGPUTextureObjectDesc &desc) :
    JzRE::JzGPUTextureObject(desc)
{
    // Convert texture type to OpenGL texture type
    m_target         = ConvertTextureType(desc.type);
    m_internalFormat = ConvertInternalFormat(desc.format);
    m_format         = ConvertFormat(desc.format);
    m_type           = ConvertType(desc.format);

    // Generate texture object
    glGenTextures(1, &m_handle);
    glBindTexture(m_target, m_handle);

    // Set texture parameters
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, ConvertFilter(desc.minFilter));
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, ConvertFilter(desc.magFilter));
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, ConvertWrap(desc.wrapS));
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, ConvertWrap(desc.wrapT));
    if (desc.type == JzETextureResourceType::Texture3D || desc.type == JzETextureResourceType::TextureCube) {
        glTexParameteri(m_target, GL_TEXTURE_WRAP_R, ConvertWrap(desc.wrapR));
    }

    // Allocate texture storage space
    switch (desc.type) {
        case JzETextureResourceType::Texture1D:
            glTexImage1D(m_target, 0, m_internalFormat, desc.width, 0, m_format, m_type, desc.data);
            break;
        case JzETextureResourceType::Texture2D:
            glTexImage2D(m_target, 0, m_internalFormat, desc.width, desc.height, 0, m_format, m_type, desc.data);
            break;
        case JzETextureResourceType::Texture3D:
            glTexImage3D(m_target, 0, m_internalFormat, desc.width, desc.height, desc.depth, 0, m_format, m_type, desc.data);
            break;
        case JzETextureResourceType::TextureCube:
            // Allocate storage space for each face of the cube texture
            for (int i = 0; i < 6; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, desc.width, desc.height, 0, m_format, m_type, nullptr);
            }
            break;
        case JzETextureResourceType::Texture2DArray:
            glTexImage3D(m_target, 0, m_internalFormat, desc.width, desc.height, desc.arraySize, 0, m_format, m_type, desc.data);
            break;
    }

    // If mipmaps are needed, generate them
    if (desc.mipLevels > 1) {
        glGenerateMipmap(m_target);
    }

    glBindTexture(m_target, 0);
}

JzRE::JzOpenGLTexture::~JzOpenGLTexture()
{
    if (m_handle != 0) {
        glDeleteTextures(1, &m_handle);
        m_handle = 0;
    }
}

void JzRE::JzOpenGLTexture::UpdateData(const void *data, JzRE::U32 mipLevel, U32 arrayIndex)
{
    if (!data) return;

    glBindTexture(m_target, m_handle);

    switch (desc.type) {
        case JzETextureResourceType::Texture1D:
            glTexSubImage1D(m_target, mipLevel, 0, desc.width >> mipLevel, m_format, m_type, data);
            break;
        case JzETextureResourceType::Texture2D:
            glTexSubImage2D(m_target, mipLevel, 0, 0, desc.width >> mipLevel, desc.height >> mipLevel, m_format, m_type, data);
            break;
        case JzETextureResourceType::Texture3D:
            glTexSubImage3D(m_target, mipLevel, 0, 0, 0, desc.width >> mipLevel, desc.height >> mipLevel, desc.depth >> mipLevel, m_format, m_type, data);
            break;
        case JzETextureResourceType::TextureCube:
            // For cube texture, arrayIndex represents the index of the face
            if (arrayIndex < 6) {
                glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + arrayIndex, mipLevel, 0, 0, desc.width >> mipLevel, desc.height >> mipLevel, m_format, m_type, data);
            }
            break;
        case JzETextureResourceType::Texture2DArray:
            glTexSubImage3D(m_target, mipLevel, 0, 0, arrayIndex, desc.width >> mipLevel, desc.height >> mipLevel, 1, m_format, m_type, data);
            break;
    }

    glBindTexture(m_target, 0);
}

void JzRE::JzOpenGLTexture::GenerateMipmaps()
{
    glBindTexture(m_target, m_handle);
    glGenerateMipmap(m_target);
    glBindTexture(m_target, 0);
}

void *JzRE::JzOpenGLTexture::GetTextureID() const
{
    return reinterpret_cast<void *>(static_cast<uintptr_t>(m_handle));
}

GLenum JzRE::JzOpenGLTexture::GetTarget() const
{
    return m_target;
}

GLenum JzRE::JzOpenGLTexture::ConvertTextureType(JzRE::JzETextureResourceType type)
{
    switch (type) {
        case JzETextureResourceType::Texture1D:
            return GL_TEXTURE_1D;
        case JzETextureResourceType::Texture2D:
            return GL_TEXTURE_2D;
        case JzETextureResourceType::Texture3D:
            return GL_TEXTURE_3D;
        case JzETextureResourceType::TextureCube:
            return GL_TEXTURE_CUBE_MAP;
        case JzETextureResourceType::Texture2DArray:
            return GL_TEXTURE_2D_ARRAY;
        default:
            return GL_TEXTURE_2D;
    }
}

GLenum JzRE::JzOpenGLTexture::ConvertInternalFormat(JzRE::JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::R8:
            return GL_R8;
        case JzETextureResourceFormat::RG8:
            return GL_RG8;
        case JzETextureResourceFormat::RGB8:
            return GL_RGB8;
        case JzETextureResourceFormat::RGBA8:
            return GL_RGBA8;
        case JzETextureResourceFormat::R16F:
            return GL_R16F;
        case JzETextureResourceFormat::RG16F:
            return GL_RG16F;
        case JzETextureResourceFormat::RGB16F:
            return GL_RGB16F;
        case JzETextureResourceFormat::RGBA16F:
            return GL_RGBA16F;
        case JzETextureResourceFormat::R32F:
            return GL_R32F;
        case JzETextureResourceFormat::RG32F:
            return GL_RG32F;
        case JzETextureResourceFormat::RGB32F:
            return GL_RGB32F;
        case JzETextureResourceFormat::RGBA32F:
            return GL_RGBA32F;
        case JzETextureResourceFormat::Depth16:
            return GL_DEPTH_COMPONENT16;
        case JzETextureResourceFormat::Depth24:
            return GL_DEPTH_COMPONENT24;
        case JzETextureResourceFormat::Depth32F:
            return GL_DEPTH_COMPONENT32F;
        case JzETextureResourceFormat::Depth24Stencil8:
            return GL_DEPTH24_STENCIL8;
        default:
            return GL_RGBA8;
    }
}

GLenum JzRE::JzOpenGLTexture::ConvertFormat(JzRE::JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::R8:
        case JzETextureResourceFormat::R16F:
        case JzETextureResourceFormat::R32F:
            return GL_RED;
        case JzETextureResourceFormat::RG8:
        case JzETextureResourceFormat::RG16F:
        case JzETextureResourceFormat::RG32F:
            return GL_RG;
        case JzETextureResourceFormat::RGB8:
        case JzETextureResourceFormat::RGB16F:
        case JzETextureResourceFormat::RGB32F:
            return GL_RGB;
        case JzETextureResourceFormat::RGBA8:
        case JzETextureResourceFormat::RGBA16F:
        case JzETextureResourceFormat::RGBA32F:
            return GL_RGBA;
        case JzETextureResourceFormat::Depth16:
        case JzETextureResourceFormat::Depth24:
        case JzETextureResourceFormat::Depth32F:
            return GL_DEPTH_COMPONENT;
        case JzETextureResourceFormat::Depth24Stencil8:
            return GL_DEPTH_STENCIL;
        default:
            return GL_RGBA;
    }
}

GLenum JzRE::JzOpenGLTexture::ConvertType(JzRE::JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::R8:
        case JzETextureResourceFormat::RG8:
        case JzETextureResourceFormat::RGB8:
        case JzETextureResourceFormat::RGBA8:
            return GL_UNSIGNED_BYTE;
        case JzETextureResourceFormat::R16F:
        case JzETextureResourceFormat::RG16F:
        case JzETextureResourceFormat::RGB16F:
        case JzETextureResourceFormat::RGBA16F:
            return GL_HALF_FLOAT;
        case JzETextureResourceFormat::R32F:
        case JzETextureResourceFormat::RG32F:
        case JzETextureResourceFormat::RGB32F:
        case JzETextureResourceFormat::RGBA32F:
        case JzETextureResourceFormat::Depth32F:
            return GL_FLOAT;
        case JzETextureResourceFormat::Depth16:
            return GL_UNSIGNED_SHORT;
        case JzETextureResourceFormat::Depth24:
            return GL_UNSIGNED_INT;
        case JzETextureResourceFormat::Depth24Stencil8:
            return GL_UNSIGNED_INT_24_8;
        default:
            return GL_UNSIGNED_BYTE;
    }
}

GLenum JzRE::JzOpenGLTexture::ConvertFilter(JzRE::JzETextureResourceFilter filter)
{
    switch (filter) {
        case JzETextureResourceFilter::Nearest:
            return GL_NEAREST;
        case JzETextureResourceFilter::Linear:
            return GL_LINEAR;
        case JzETextureResourceFilter::NearestMipmapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case JzETextureResourceFilter::LinearMipmapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case JzETextureResourceFilter::NearestMipmapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case JzETextureResourceFilter::LinearMipmapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            return GL_LINEAR;
    }
}

GLenum JzRE::JzOpenGLTexture::ConvertWrap(JzRE::JzETextureResourceWrap wrap)
{
    switch (wrap) {
        case JzETextureResourceWrap::Repeat:
            return GL_REPEAT;
        case JzETextureResourceWrap::MirroredRepeat:
            return GL_MIRRORED_REPEAT;
        case JzETextureResourceWrap::ClampToEdge:
            return GL_CLAMP_TO_EDGE;
        case JzETextureResourceWrap::ClampToBorder:
            return GL_CLAMP_TO_BORDER;
        default:
            return GL_REPEAT;
    }
}