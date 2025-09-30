/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <glad/glad.h>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHITexture.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Texture
 */
class JzOpenGLTexture : public JzRHITexture {
public:
    /**
     * @brief Constructor
     *
     * @param desc The description of the texture
     */
    JzOpenGLTexture(const JzTextureDesc &desc);

    /**
     * @brief Destructor
     */
    ~JzOpenGLTexture() override;

    /**
     * @brief Update the data of the texture
     *
     * @param data The data to update
     * @param mipLevel The mip level to update
     * @param arrayIndex The array index to update
     */
    void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) override;

    /**
     * @brief Generate mipmaps
     */
    void GenerateMipmaps() override;

    /**
     * @brief Get the native texture ID for rendering
     *
     * @return The native texture ID
     */
    void *GetTextureID() const override;

    /**
     * @brief Get the target of the texture
     *
     * @return The target of the texture
     */
    GLenum GetTarget() const;

private:
    /**
     * @brief Convert texture type to OpenGL texture type
     *
     * @param type The texture type
     * @return The OpenGL texture type
     */
    static GLenum ConvertTextureType(JzETextureType type);

    /**
     * @brief Convert internal format to OpenGL internal format
     *
     * @param format The texture format
     * @return The OpenGL internal format
     */
    static GLenum ConvertInternalFormat(JzETextureFormat format);

    /**
     * @brief Convert format to OpenGL format
     *
     * @param format The texture format
     * @return The OpenGL format
     */
    static GLenum ConvertFormat(JzETextureFormat format);

    /**
     * @brief Convert type to OpenGL type
     *
     * @param format The texture format
     * @return The OpenGL type
     */
    static GLenum ConvertType(JzETextureFormat format);

    /**
     * @brief Convert filter to OpenGL filter
     *
     * @param filter The texture filter
     * @return The OpenGL filter
     */
    static GLenum ConvertFilter(JzETextureFilter filter);

    /**
     * @brief Convert wrap to OpenGL wrap
     *
     * @param wrap The texture wrap
     * @return The OpenGL wrap
     */
    static GLenum ConvertWrap(JzETextureWrap wrap);

private:
    GLuint m_handle = 0;
    GLenum m_target;
    GLenum m_internalFormat;
    GLenum m_format;
    GLenum m_type;
};

} // namespace JzRE